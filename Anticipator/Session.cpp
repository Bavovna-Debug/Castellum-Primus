// System definition files.
//
#include <unistd.h>
#include <cstdbool>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>
#include <thread>

// Common definition files.
//
#include "APNS/APNS.hpp"
#include "Communicator/TCP.hpp"
#include "RTSP/RTSP.hpp"
#include "Toolkit/Report.h"

// Local definition files.
//
#include "Primus/Configuration.hpp"
#include "Primus/Anticipator/Session.hpp"
#include "Primus/Database/Debug.hpp"
#include "Primus/Database/Phoenix.hpp"
#include "Primus/Database/Phoenixes.hpp"

Anticipator::Session::Session(TCP::Service& service) :
Inherited(service)
{
    // Allocate resources to be used for receive buffer.
    //
    this->receiveBuffer = (char*) malloc(Anticipator::BytesReceivePerStep);
    if (this->receiveBuffer == NULL)
    {
        ReportSoftAlert("[Anticipator] Out of memory");

        throw std::runtime_error("[Anticipator] Out of memory");
    }
}

Anticipator::Session::~Session()
{
    free(this->receiveBuffer);
}

void
Anticipator::Session::ThreadHandler(Anticipator::Session* session)
{
    ReportInfo("[Anticipator] Started thread to process session");

    Primus::Configuration& configuration = Primus::Configuration::SharedInstance();

    unsigned long debugSessionId = Primus::Debug::BeginPhoenixSession(session->remoteAddress);

    bool sessionOK = true;

    // Session should begin with CSeq 1 incrementing for each new datagram.
    // Any other value means that either Phoenix had a problem or be interpreted as intrusion attack.
    //
    unsigned int expectedCSeq = 1;

    // Wait for the beginning of transmission (it should not explicitly begin immediately).
    // Cancel session in case of timeout.
    //
    try
    {
        Communicator::Poll(
                session->socket(),
                configuration.phoenix.waitForFirstDatagram);
    }
    catch (Communicator::PollError&)
    {
        Primus::Debug::CommentPhoenixSession(
                debugSessionId,
                "Poll for transmission did break");

        goto out;
    }
    catch (Communicator::PollTimeout&)
    {
        Primus::Debug::CommentPhoenixSession(
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
    // Break the loop if session is timed out (no datagram exchange for a long time).
    //
    do
    {
        RTSP::Datagram request;
        RTSP::Datagram response;

        // Receive datagram chunks continuously until either complete datagram is received
        // or timeout ocures.
        //
        for (;;)
        {
            unsigned int receivedBytes;

            try
            {
                receivedBytes = Communicator::Receive(
                        session->socket(),
                        session->receiveBuffer,
                        Anticipator::BytesReceivePerStep);
            }
            catch (Communicator::TransmissionError& exception)
            {
                ReportWarning("[Anticipator] Session is broken: errno=%d",
                        exception.errorNumber);

                Primus::Debug::CommentPhoenixSession(
                        debugSessionId,
                        "Session is broken");

                goto out;
            }
            catch (Communicator::NothingReceived&)
            {
                ReportDebug("[Anticipator] Disconnected");

                Primus::Debug::CommentPhoenixSession(
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
                ReportWarning("[Anticipator] Rejected: %s", exception.what());

                goto out;
            }

            if (request.headerComplete == true)
                break;

            // Wait until next chunk of datagram is available.
            // Cancel session in case of timeout.
            //
            try
            {
                Communicator::Poll(
                        session->socket(),
                        configuration.phoenix.waitForDatagramCompletion);
            }
            catch (Communicator::PollError&)
            {
                Primus::Debug::CommentPhoenixSession(
                        debugSessionId,
                        "Poll for chunk did break");

                goto out;
            }
            catch (Communicator::PollTimeout&)
            {
                Primus::Debug::CommentPhoenixSession(
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
                    std::this_thread::sleep_for(std::chrono::milliseconds
                            { configuration.phoenix.delayResponseForRejected } );

                    response.reset();
                    response["CSeq"] = expectedCSeq;
                    response["Agent"] = Primus::SoftwareVersion;
                    response["Reason"] = "Unexpected CSeq";
                    response.generateResponse(RTSP::BadRequest);

                    throw Anticipator::RejectDatagram("Unexpected CSeq");
                }
            }
            catch (RTSP::StatementNotFound& exception)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds
                        { configuration.phoenix.delayResponseForRejected } );

                response.reset();
                response["CSeq"] = expectedCSeq;
                response["Agent"] = Primus::SoftwareVersion;
                response["Reason"] = "Missing CSeq";
                response.generateResponse(RTSP::BadRequest);

                throw Anticipator::RejectDatagram("Missing CSeq");
            }

            if (request.methodIs("ACTIVATE") == true)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds
                        { configuration.phoenix.delayResponseForActivate } );

                try
                {
                    const std::string softwareVersion = request["Software-Version"];
                    const std::string activationCode = request["Activation-Code"];
                    const std::string vendorToken = request["Vendor-Token"];
                    const std::string deviceName = request["Device-Name"];
                    const std::string deviceModel = request["Device-Model"];

                    if (softwareVersion.length() == 0)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds
                                { configuration.phoenix.delayResponseForRejected } );

                        response.reset();
                        response["CSeq"] = expectedCSeq;
                        response["Agent"] = Primus::SoftwareVersion;
                        response["Reason"] = "Missing software version";
                        response.generateResponse(RTSP::BadRequest);

                        throw Anticipator::RejectDatagram("Missing software version");
                    }

                    if (activationCode.length() != Primus::ActivationCodeLength)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds
                                { configuration.phoenix.delayResponseForRejected } );

                        response.reset();
                        response["CSeq"] = expectedCSeq;
                        response["Agent"] = Primus::SoftwareVersion;
                        response["Reason"] = "Activation code in wrong format";
                        response.generateResponse(RTSP::NotAcceptable);

                        throw Anticipator::RejectDatagram("Activation code in wrong format");
                    }

                    if (vendorToken.length() != PostgreSQL::UUIDPlainLength)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds
                                { configuration.phoenix.delayResponseForRejected } );

                        response.reset();
                        response["CSeq"] = expectedCSeq;
                        response["Agent"] = Primus::SoftwareVersion;
                        response["Reason"] = "Bad ventor token";
                        response.generateResponse(RTSP::BadRequest);

                        throw Anticipator::RejectDatagram("Bad ventor token");
                    }

                    if (deviceName.length() == 0)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds
                                { configuration.phoenix.delayResponseForRejected } );

                        response.reset();
                        response["CSeq"] = expectedCSeq;
                        response["Agent"] = Primus::SoftwareVersion;
                        response["Reason"] = "Missing device name";
                        response.generateResponse(RTSP::BadRequest);

                        throw Anticipator::RejectDatagram("Missing device name");
                    }

                    if (deviceModel.length() == 0)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds
                                { configuration.phoenix.delayResponseForRejected } );

                        response.reset();
                        response["CSeq"] = expectedCSeq;
                        response["Agent"] = Primus::SoftwareVersion;
                        response["Reason"] = "Missing device model";
                        response.generateResponse(RTSP::BadRequest);

                        throw Anticipator::RejectDatagram("Missing device model");
                    }

                    unsigned long phoenixId = Database::Phoenix::RegisterPhoenixWithActivationCode(
                            activationCode,
                            vendorToken,
                            deviceName,
                            deviceModel,
                            softwareVersion,
                            deviceName);

                    if (phoenixId == 0)
                    {
                        response.reset();
                        response["CSeq"] = expectedCSeq;
                        response["Agent"] = Primus::SoftwareVersion;
                        response.generateResponse(RTSP::NotFound);
                    }
                    else
                    {
                        session->phoenix = &Database::Phoenixes::PhoenixById(phoenixId);

                        response.reset();
                        response["CSeq"] = expectedCSeq;
                        response["Agent"] = Primus::SoftwareVersion;
                        response["Walker-Token"] = session->phoenix->token;
                        response.generateResponse(RTSP::OK);
                    }
                }
                catch (RTSP::StatementNotFound& exception)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds
                            { configuration.phoenix.delayResponseForRejected } );

                    response.reset();
                    response["CSeq"] = expectedCSeq;
                    response["Agent"] = Primus::SoftwareVersion;
                    response["Reason"] = "Missing mandatory statements";
                    response.generateResponse(RTSP::BadRequest);

                    throw Anticipator::RejectDatagram("Missing statements");
                }
            }
            else if (request.methodIs("APNS") == true)
            {
                const std::string deviceToken = request["Device-Token"];

                char t[72];
                APNS::DeviceTokenFromString(t, deviceToken.c_str());
                APNS::DeviceTokenSymbolToBin(session->phoenix->deviceToken, t);

                session->phoenix->saveDeviceToken();

                response.reset();
                response["CSeq"] = expectedCSeq;
                response["Agent"] = Primus::SoftwareVersion;
                response.generateResponse(RTSP::Created);
            }
            else if (request.methodIs("LOGIN") == true)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds
                        { configuration.phoenix.delayResponseForLogin } );

                try
                {
                    const std::string phoenixToken = request["Walker-Token"];
                    const std::string softwareVersion = request["Software-Version"];

                    if (phoenixToken.length() == 0)
                    {
                        response.reset();
                        response["CSeq"] = expectedCSeq;
                        response["Agent"] = Primus::SoftwareVersion;
                        response["Reason"] = "Bad phoenix token";
                        response.generateResponse(RTSP::BadRequest);

                        throw Anticipator::RejectDatagram("Bad phoenix token");
                    }

                    if (phoenixToken.length() != PostgreSQL::UUIDPlainLength)
                    {
                        response.reset();
                        response["CSeq"] = expectedCSeq;
                        response["Agent"] = Primus::SoftwareVersion;
                        response["Reason"] = "Walker token in wrong format";
                        response.generateResponse(RTSP::BadRequest);

                        throw Anticipator::RejectDatagram("Walker token in wrong format");
                    }

                    if (softwareVersion.length() == 0)
                    {
                        response.reset();
                        response["CSeq"] = expectedCSeq;
                        response["Agent"] = Primus::SoftwareVersion;
                        response["Reason"] = "Missing software version";
                        response.generateResponse(RTSP::BadRequest);

                        throw Anticipator::RejectDatagram("Missing software version");
                    }

                    session->phoenix = &Database::Phoenixes::PhoenixByToken(phoenixToken);

                    session->phoenix->setSoftwareVersion(softwareVersion);

                    response.reset();
                    response["CSeq"] = expectedCSeq;
                    response["Agent"] = Primus::SoftwareVersion;
                    response.generateResponse(RTSP::OK);
                }
                catch (RTSP::StatementNotFound& exception)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds
                            { configuration.phoenix.delayResponseForRejected } );

                    response.reset();
                    response["CSeq"] = expectedCSeq;
                    response["Agent"] = Primus::SoftwareVersion;
                    response["Reason"] = "Missing mandatory statements";
                    response.generateResponse(RTSP::BadRequest);

                    throw Anticipator::RejectDatagram("Missing statements");
                }
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds
                        { configuration.phoenix.delayResponseForRejected } );

                response.reset();
                response["CSeq"] = expectedCSeq;
                response["Agent"] = Primus::SoftwareVersion;
                response.generateResponse(RTSP::MethodNotAllowed);

                throw Anticipator::RejectDatagram("Unknown method");
            }
        }
        catch (APNS::BrokenDeviceToken& exception)
        {
            ReportWarning("[Anticipator] Rejected: %s", exception.what());

            Primus::Debug::CommentPhoenixSession(debugSessionId, exception.what());

            sessionOK = false;
        }
        catch (Anticipator::RejectDatagram& exception)
        {
            ReportWarning("[Anticipator] Rejected: %s", exception.what());

            Primus::Debug::CommentPhoenixSession(debugSessionId, exception.what());

            sessionOK = false;
        }
        catch (std::exception& exception)
        {
            ReportWarning("[Anticipator] Rejected: %s", exception.what());

            Primus::Debug::CommentPhoenixSession(debugSessionId, exception.what());

            sessionOK = false;
        }

        try
        {
            Communicator::Send(
                    session->socket(),
                    response.payloadBuffer,
                    response.payloadLength);
        }
        catch (...)
        { }

        Primus::Debug::ReportPhoenixRTSP(debugSessionId, request, response);

        // Wait until next datagram is available.
        // Cancel session in case of timeout.
        // If polling breaks due to new data available, then just go further
        // to the beginning of the loop.
        //
        try
        {
            Communicator::Poll(
                    session->socket(),
                    configuration.phoenix.keepAlive);
        }
        catch (Communicator::PollError&)
        {
            Primus::Debug::CommentPhoenixSession(
                    debugSessionId,
                    "Keep-alive polling did break");

            goto out;
        }
        catch (Communicator::PollTimeout&)
        {
            Primus::Debug::CommentPhoenixSession(
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
    Primus::Debug::ClosePhoenixSession(debugSessionId);

    ReportInfo("[Anticipator] Thread is going to quit");

    delete session;
}
