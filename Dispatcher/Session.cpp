// System definition files.
//
#include <chrono>
#include <cstdlib>
#include <sstream>
#include <thread>

// Common definition files.
//
#include "Communicator/TCP.hpp"
#include "RTSP/RTSP.hpp"
#include "Toolkit/Report.h"
#include "Toolkit/Times.hpp"

// Local definition files.
//
#include "Primus/Configuration.hpp"
#include "Primus/Database/Debug.hpp"
#include "Primus/Database/Fabula.hpp"
#include "Primus/Database/Servus.hpp"
#include "Primus/Database/Servuses.hpp"
#include "Primus/Database/Thermas.hpp"
#include "Primus/Dispatcher/Listener.hpp"
#include "Primus/Dispatcher/Notificator.hpp"
#include "Primus/Dispatcher/Session.hpp"

Dispatcher::Session::Session(TCP::Service& service) :
Inherited(service)
{
    // Allocate resources to be used for receive buffer.
    //
    this->receiveBuffer = (char*) malloc(Dispatcher::BytesReceivePerStep);
    if (this->receiveBuffer == NULL)
    {
        ReportSoftAlert("[Dispatcher] Out of memory");

        throw std::runtime_error("[Dispatcher] Out of memory");
    }

    this->servus = nullptr;
}

Dispatcher::Session::~Session()
{
    if (this->servus != nullptr)
        delete this->servus;

    free(this->receiveBuffer);
}

void
Dispatcher::Session::ThreadHandler(Dispatcher::Session* session)
{
    ReportInfo("[Dispatcher] Started thread to process session");

    Primus::Configuration& configuration = Primus::Configuration::SharedInstance();

    Dispatcher::Notificator& notificator = Dispatcher::Notificator::SharedInstance();

    unsigned long debugSessionId = Primus::Debug::BeginServusSession(session->remoteAddress);

    bool sessionOK = true;

    // Session should begin with CSeq 1 incrementing for each new datagram.
    // Any other value means that either servus had a problem or be interpreted as intrusion attack.
    //
    unsigned int expectedCSeq = 1;

    RTSP::Datagram request;
    RTSP::Datagram response;

    // Wait for the beginning of transmission (it should not explicitly begin immediately).
    // Cancel session in case of timeout.
    //
    try
    {
        Communicator::Poll(
                session->socket(),
                configuration.servus.waitForFirstDatagram);
    }
    catch (Communicator::PollError&)
    {
        Primus::Debug::CommentServusSession(
                debugSessionId,
                "Poll for transmission did break");

        goto out;
    }
    catch (Communicator::PollTimeout&)
    {
        Primus::Debug::CommentServusSession(
                debugSessionId,
                "Poll for transmission timed out");

        goto out;
    }

    // Manage an endless loop of:
    //   - Receive datagram - if necessary receive several chunks until datagram is complete.
    //   - Process datagram.
    //   - Generate response.
    //   - Send response.
    //   - Store debug informations.
    //   - Periodically send neutrinos to servus to make sure it is still alive.
    // Break the loop if session is timed out (no datagram exchange for a long time).
    //
    do
    {
        request.reset();

        // Receive datagram chunks continuously until either complete datagram
        // is received or timeout ocures.
        //
        for (;;)
        {
            unsigned int receivedBytes;

            try
            {
                receivedBytes = Communicator::Receive(
                        session->socket(),
                        session->receiveBuffer,
                        Dispatcher::BytesReceivePerStep);
            }
            catch (Communicator::TransmissionError& exception)
            {
                ReportWarning("[Dispatcher] Connection is broken: errno=%d",
                        exception.errorNumber);

                Primus::Debug::CommentServusSession(
                        debugSessionId,
                        "Connection is broken");

                goto out;
            }
            catch (Communicator::NothingReceived&)
            {
                ReportDebug("[Dispatcher] Disconnected");

                Primus::Debug::CommentServusSession(
                        debugSessionId,
                        "Disconnected");

                goto out;
            }

            try
            {
                request.push(session->receiveBuffer, receivedBytes);
            }
            catch (std::exception& exception)
            {
                ReportWarning("[Dispatcher] Rejected: %s", exception.what());

                goto out;
            }

            if (request.datagramComplete() == true)
            {
                break;
            }

            // Wait until next chunk of datagram is available.
            // Cancel session in case of timeout.
            //
            try
            {
                Communicator::Poll(
                        session->socket(),
                        configuration.servus.waitForDatagramCompletion);
            }
            catch (Communicator::PollError&)
            {
                Primus::Debug::CommentServusSession(
                        debugSessionId,
                        "Poll for chunk did break");

                goto out;
            }
            catch (Communicator::PollTimeout&)
            {
                Primus::Debug::CommentServusSession(
                        debugSessionId,
                        "Poll for chunk timed out");

                goto out;
            }
        }

        try
        {
            try
            {
                const unsigned int providedCSeq = request["CSeq"];

                if (providedCSeq != expectedCSeq)
                {
                    response.reset();
                    response["CSeq"] = expectedCSeq;
                    response["Agent"] = Primus::SoftwareVersion;
                    response["Reason"] = "Unexpected CSeq";
                    response.generateResponse(RTSP::BadRequest);

                    throw Dispatcher::RejectDatagram("Unexpected CSeq");
                }
            }
            catch (RTSP::StatementNotFound& exception)
            {
                response.reset();
                response["CSeq"] = expectedCSeq;
                response["Agent"] = Primus::SoftwareVersion;
                response["Reason"] = "Missing CSeq";
                response.generateResponse(RTSP::BadRequest);

                throw Dispatcher::RejectDatagram("Missing CSeq");
            }

            if (request.methodIs("AUTH") == true)
            {
                const std::string authenticator = request["Authenticator"];
                if (authenticator.length() == 0)
                {
                    response.reset();
                    response["CSeq"] = expectedCSeq;
                    response["Agent"] = Primus::SoftwareVersion;
                    response.generateResponse(RTSP::Unauthorized);

                    throw Dispatcher::RejectDatagram("Missing authenticator");
                }
                else if (authenticator.length() != PostgreSQL::UUIDPlainLength)
                {
                    response.reset();
                    response["CSeq"] = expectedCSeq;
                    response["Agent"] = Primus::SoftwareVersion;
                    response.generateResponse(RTSP::Unauthorized);

                    throw Dispatcher::RejectDatagram("Bad authenticator");
                }

                try
                {
                    session->servus = &Database::Servuses::ServusByAuthenticator(authenticator);

                    session->servus->setOnline();
                }
                catch (Database::ServusNotFound&)
                {
                    response.reset();
                    response["CSeq"] = expectedCSeq;
                    response["Agent"] = Primus::SoftwareVersion;
                    response.generateResponse(RTSP::Unauthorized);

                    ReportInfo("[Dispatcher] Invalid authenticator provided: %s",
                            authenticator.c_str());

                    throw Dispatcher::RejectDatagram("Invalid authenticator");
                }

                try
                {
                    const std::string* originStamp = request["Running-Since"];

                    Toolkit::Timestamp runningSince(*originStamp);

                    session->servus->setRunningSince(runningSince);
                }
                catch (RTSP::StatementNotFound& exception)
                {
                    // Running-since statement is optional.
                }

                if (session->servus->enabled == false)
                {
                    response.reset();
                    response["CSeq"] = expectedCSeq;
                    response["Agent"] = Primus::SoftwareVersion;
                    response.generateResponse(RTSP::Forbidden);

                    ReportInfo("[Dispatcher] Desabled servus tries to connect: %s",
                            session->servus->token.c_str());

                    throw Dispatcher::RejectDatagram("Servus disabled");
                }

                ReportInfo("[Dispatcher] Authentificated servus \"%s\"",
                        session->servus->description.c_str());

                response.reset();
                response["CSeq"] = expectedCSeq;
                response["Agent"] = Primus::SoftwareVersion;
                response["Neutrino-Interval"] =
                        configuration.servus.intervalBetweenNeutrinos;
                response.generateResponse(RTSP::OK);
            }
            else if (request.methodIs("SETUP") == true)
            {
                ReportInfo("[Dispatcher] Servus \"%s\" requested configuration",
                        session->servus->description.c_str());

                const std::string configurationJSON = session->servus->configurationJSON();

                response.reset();
                response["CSeq"] = expectedCSeq;
                response["Agent"] = Primus::SoftwareVersion;
                response["Neutrino-Interval"] =
                        configuration.servus.intervalBetweenNeutrinos;
                response.generateResponse(RTSP::OK, configurationJSON);
            }
            else if (request.methodIs("PLAY") == true)
            {
                ReportInfo("[Dispatcher] Servus \"%s\" started measurement",
                        session->servus->description.c_str());

                response.reset();
                response["CSeq"] = expectedCSeq;
                response["Agent"] = Primus::SoftwareVersion;
                response["Neutrino-Interval"] =
                        configuration.servus.intervalBetweenNeutrinos;
                response.generateResponse(RTSP::Continue);
            }
            else if (request.methodIs("NEUTRINO") == true)
            {
                ReportDebug("[Dispatcher] Received neutrino");

                response.reset();
                response["CSeq"] = expectedCSeq;
                response["Agent"] = Primus::SoftwareVersion;
                response["Neutrino-Interval"] =
                        configuration.servus.intervalBetweenNeutrinos;
                response.generateResponse(RTSP::Continue);
            }
            else if (request.methodIs("AVISO") == true)
            {
                ReportDebug("[Dispatcher] Received aviso");

                try
                {
                    unsigned int    avisoId;
                    std::string*    originStamp;
                    unsigned short  severity;
                    std::string*    originator;

                    try
                    {
                        avisoId = request["Aviso-Id"];
                    }
                    catch (RTSP::StatementNotFound& exception)
                    {
                        response.reset();
                        response["CSeq"] = expectedCSeq;
                        response["Agent"] = Primus::SoftwareVersion;
                        response["Neutrino-Interval"] =
                                configuration.servus.intervalBetweenNeutrinos;
                        response.generateResponse(RTSP::NotAcceptable);

                        throw Dispatcher::RejectDatagram("[Dispatcher] Missing 'Aviso-Id'");
                    }

                    try
                    {
                        originStamp = request["Timestamp"];
                        severity = request["Severity"];
                    }
                    catch (RTSP::StatementNotFound& exception)
                    {
                        response.reset();
                        response["CSeq"] = expectedCSeq;
                        response["Agent"] = Primus::SoftwareVersion;
                        response["Aviso-Id"] = avisoId;
                        response["Neutrino-Interval"] =
                                configuration.servus.intervalBetweenNeutrinos;
                        response.generateResponse(RTSP::NotAcceptable);

                        throw Dispatcher::RejectDatagram("[Dispatcher] Missing 'Timestamp' or 'Severity'");
                    }

                    try
                    {
                        originator = request["Originator"];

                        if (originator->empty() == true)
                        {
                            response.reset();
                            response["CSeq"] = expectedCSeq;
                            response["Agent"] = Primus::SoftwareVersion;
                            response["Aviso-Id"] = avisoId;
                            response["Neutrino-Interval"] =
                                    configuration.servus.intervalBetweenNeutrinos;
                            response.generateResponse(RTSP::NotAcceptable);

                            throw Dispatcher::RejectDatagram("[Dispatcher] Empty 'Originator'");
                        }
                    }
                    catch (RTSP::StatementNotFound& exception)
                    {
                        response.reset();
                        response["CSeq"] = expectedCSeq;
                        response["Agent"] = Primus::SoftwareVersion;
                        response["Aviso-Id"] = avisoId;
                        response["Neutrino-Interval"] =
                                configuration.servus.intervalBetweenNeutrinos;
                        response.generateResponse(RTSP::NotAcceptable);

                        throw Dispatcher::RejectDatagram("[Dispatcher] Missing 'Originator'");
                    }

                    if (request.payloadLength() == 0)
                    {
                        response.reset();
                        response["CSeq"] = expectedCSeq;
                        response["Agent"] = Primus::SoftwareVersion;
                        response["Aviso-Id"] = avisoId;
                        response["Neutrino-Interval"] =
                                configuration.servus.intervalBetweenNeutrinos;
                        response.generateResponse(RTSP::NotAcceptable);

                        throw Dispatcher::RejectDatagram("[Dispatcher] Missing payload");
                    }

                    Toolkit::Timestamp timestamp(*originStamp);

                    std::string payload = request.payload();

                    Database::Fabula::Enqueue(
                            timestamp,
                            session->servus->servusId,
                            *originator,
                            severity,
                            payload);

                    response.reset();
                    response["CSeq"] = expectedCSeq;
                    response["Agent"] = Primus::SoftwareVersion;
                    response["Aviso-Id"] = avisoId;
                    response["Neutrino-Interval"] =
                            configuration.servus.intervalBetweenNeutrinos;
                    response.generateResponse(RTSP::Created);

                    notificator.triggerProcessing();
                }
                catch (std::exception& exception)
                {
                    ReportError("[Dispatcher] Cannot process aviso: %s",
                            exception.what());

                    response.reset();
                    response["CSeq"] = expectedCSeq;
                    response["Agent"] = Primus::SoftwareVersion;
                    response["Neutrino-Interval"] =
                            configuration.servus.intervalBetweenNeutrinos;
                    response.generateResponse(RTSP::NotAcceptable);
                }
            }
            else if (request.methodIs("TEMPERATURE") == true)
            {
                ReportDebug("[Dispatcher] Received temperature");

                try
                {
                    const unsigned int avisoId      = request["Aviso-Id"];
                    const std::string originStamp   = request["Timestamp"];
                    const std::string sensorToken   = request["Sensor-Token"];
                    const float temperature         = request["Temperature"];

                    Toolkit::Timestamp timestamp(originStamp);

                    Database::NoticeTemperature(
                            timestamp,
                            sensorToken,
                            std::stod(originStamp),
                            temperature);

                    response.reset();
                    response["CSeq"] = expectedCSeq;
                    response["Agent"] = Primus::SoftwareVersion;
                    response["Aviso-Id"] = avisoId;
                    response["Neutrino-Interval"] =
                            configuration.servus.intervalBetweenNeutrinos;
                    response.generateResponse(RTSP::Created);
                }
                catch (std::exception& exception)
                {
                    ReportError("[Dispatcher] Cannot process temperature: %s",
                            exception.what());

                    response.reset();
                    response["CSeq"] = expectedCSeq;
                    response["Agent"] = Primus::SoftwareVersion;
                    response["Neutrino-Interval"] =
                            configuration.servus.intervalBetweenNeutrinos;
                    response.generateResponse(RTSP::NotAcceptable);
                }
            }
            else
            {
                response.reset();
                response["CSeq"] = expectedCSeq;
                response["Agent"] = Primus::SoftwareVersion;
                response.generateResponse(RTSP::MethodNotAllowed);

                throw Dispatcher::RejectDatagram("Unknown method");
            }
        }
        catch (Dispatcher::RejectDatagram& exception)
        {
            ReportWarning("[Dispatcher] Rejected: %s", exception.what());

            Primus::Debug::CommentServusSession(debugSessionId, exception.what());

            sessionOK = false;
        }

        try
        {
            Communicator::Send(
                    session->socket(),
                    response.contentBuffer,
                    response.contentLength);
        }
        catch (...)
        { }

        Primus::Debug::ReportServusRTSP(debugSessionId, request, response);

        if (sessionOK == false)
            break;

        // Wait until next datagram is available.
        // Cancel session in case of timeout.
        // If polling breaks due to new data available, then just go further
        // to the beginning of the loop.
        //
        try
        {
            Communicator::Poll(
                    session->socket(),
                    configuration.servus.intervalBetweenNeutrinos +
                    configuration.servus.finalWaitForNeutrino);
        }
        catch (Communicator::PollError&)
        {
            Primus::Debug::CommentServusSession(
                    debugSessionId,
                    "Keep-alive polling did break");

            goto out;
        }
        catch (Communicator::PollTimeout&)
        {
            Primus::Debug::CommentServusSession(
                    debugSessionId,
                    "Session timed out");

            goto out;
        }

        // CSeq for each new datagram should be incremented by one.
        //
        expectedCSeq++;
    }
    while (sessionOK == true);

out:
    if (session->servus != nullptr)
    {
        session->servus->setOffline();
    }

    Primus::Debug::CloseServusSession(debugSessionId);

    ReportInfo("[Dispatcher] Thread is going to quit");

    delete session;
}
