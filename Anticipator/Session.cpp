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

Anticipator::Session::Session(TCP::Service& service) :
Inherited(service),
request(),
response()
{
    // Session should begin with CSeq 1 incrementing for each new datagram.
    // Any other value means that either Walker had a problem or be interpreted as intrusion attack.
    //
    this->expectedCSeq = 1;

    this->phoenix = nullptr;

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
        session->request.reset();

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
                session->request.push(session->receiveBuffer, receivedBytes);
            }
            catch (std::exception& exception)
            {
                ReportWarning("[Anticipator] Rejected: %s", exception.what());

                goto out;
            }

            if (session->request.headerComplete == true)
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
                const unsigned int providedCSeq = session->request["CSeq"];

                if (providedCSeq != session->expectedCSeq)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds
                            { configuration.phoenix.delayResponseForRejected } );

                    session->response.reset();
                    session->response["CSeq"] = session->expectedCSeq;
                    session->response["Agent"] = Primus::SoftwareVersion;
                    session->response["Reason"] = "Unexpected CSeq";
                    session->response.generateResponse(RTSP::BadRequest);

                    throw Anticipator::RejectDatagram("Unexpected CSeq");
                }
            }
            catch (RTSP::StatementNotFound& exception)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds
                        { configuration.phoenix.delayResponseForRejected } );

                session->response.reset();
                session->response["CSeq"] = session->expectedCSeq;
                session->response["Agent"] = Primus::SoftwareVersion;
                session->response["Reason"] = "Missing CSeq";
                session->response.generateResponse(RTSP::BadRequest);

                throw Anticipator::RejectDatagram("Missing CSeq");
            }

            if (session->request.methodIs("ACTIVATE") == true)
            {
                session->handleActivate();
            }
            else if (session->request.methodIs("APNS") == true)
            {
                session->handleAPNS();
            }
            else if (session->request.methodIs("LOGIN") == true)
            {
                session->handleLogin();
            }
            else if (session->request.methodIs("QUASAR-LIST") == true)
            {
                session->handleServusList();
            }
            else if (session->request.methodIs("FABULA-LIST") == true)
            {
                session->handleFabulaList();
            }
            else
            {
                session->handleUnknown();
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
                    session->response.contentBuffer,
                    session->response.contentLength);
        }
        catch (...)
        { }

        Primus::Debug::ReportPhoenixRTSP(debugSessionId, session->request, session->response);

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

            sessionOK = false;
        }
        catch (Communicator::PollTimeout&)
        {
            Primus::Debug::CommentPhoenixSession(
                    debugSessionId,
                    "Session timed out");

            sessionOK = false;
        }

        // CSeq for each new datagram should be incremented by one.
        //
        session->expectedCSeq++;
    }
    while (sessionOK == true);

out:
    Primus::Debug::ClosePhoenixSession(debugSessionId);

    ReportInfo("[Anticipator] Thread is going to quit");

    delete session;
}
