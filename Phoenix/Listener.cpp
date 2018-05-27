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
#include "Primus/Database/Debug.hpp"
#include "Primus/Database/Phoenix.hpp"
#include "Primus/Database/Phoenixes.hpp"
#include "Primus/Phoenix/Listener.hpp"

Phoenix::Listener::Listener(
    const IP::Family        family,
    const unsigned short    portNumber) :
Inherited(family, "", portNumber)
{
    this->thread = std::thread(&Phoenix::Listener::ThreadHandler, this);
}

/**
 * @brief   Thread handler for service.
 */
void
Phoenix::Listener::ThreadHandler(Phoenix::Listener* listener)
{
    ReportNotice("[Phoenix] Service thread has been started");

    // Endless loop. In case an error on socket layer occurs,
    // the socket recovery will start automatically
    // at the beginning of the loop.
    //
    for (;;)
    {
        try
        {
            listener->disconnect();
            listener->connect();

            for (;;)
            {
                Phoenix::Connection* connection = new Phoenix::Connection { *listener };

                std::thread connectionThread
                {
                    &Phoenix::Connection::ThreadHandler,
                    connection
                };

                connectionThread.detach();
            }
        }
        catch (Communicator::SocketError& exception)
        {
            ReportError("[Phoenix] Exception: %s: errno=%d",
                    exception.what(),
                    exception.errorNumber);

            continue;
        }
        catch (std::exception& exception)
        {
            ReportError("[Phoenix] Exception: %s",
                    exception.what());

            continue;
        }
    }

    // Make sure the socket is closed.
    //
    listener->disconnect();

    ReportWarning("[Phoenix] Service thread is going to quit");
}

Phoenix::Connection::Connection(TCP::Service& service) :
Inherited(service)
{
    // Allocate resources to be used for receive buffer.
    //
    this->receiveBuffer = (char*) malloc(Phoenix::BytesReceivePerStep);
    if (this->receiveBuffer == NULL)
    {
        ReportSoftAlert("[Phoenix] Out of memory");

        throw std::runtime_error("[Phoenix] Out of memory");
    }
}

Phoenix::Connection::~Connection()
{
    free(this->receiveBuffer);
}

void
Phoenix::Connection::ThreadHandler(Phoenix::Connection* connection)
{
    ReportInfo("[Phoenix] Started thread to process connection");

    Primus::Configuration& configuration = Primus::Configuration::SharedInstance();

    unsigned long debugSessionId = Primus::Debug::BeginPhoenixSession(connection->remoteAddress);

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
                connection->socket(),
                configuration.network.phoenix.waitForFirstDatagram);
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
                        connection->socket(),
                        connection->receiveBuffer,
                        Phoenix::BytesReceivePerStep);
            }
            catch (Communicator::TransmissionError& exception)
            {
                ReportWarning("[Phoenix] Connection is broken: errno=%d",
                        exception.errorNumber);

                Primus::Debug::CommentPhoenixSession(
                        debugSessionId,
                        "Connection is broken");

                goto out;
            }
            catch (Communicator::NothingReceived&)
            {
                ReportDebug("[Phoenix] Disconnected");

                Primus::Debug::CommentPhoenixSession(
                        debugSessionId,
                        "Disconnected");

                goto out;
            }

            try
            {
                request.push(connection->receiveBuffer, receivedBytes);
            }
            catch (std::exception& exception)
            {
                ReportWarning("[Phoenix] Rejected: %s", exception.what());

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
                        connection->socket(),
                        configuration.network.phoenix.waitForDatagramCompletion);
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
                    response.reset();
                    response["CSeq"] = expectedCSeq;
                    response["Agent"] = Primus::SoftwareVersion;
                    response["Reason"] = "Unexpected CSeq";
                    response.generateResponse(RTSP::BadRequest);

                    throw Phoenix::RejectDatagram("Unexpected CSeq");
                }
            }
            catch (RTSP::StatementNotFound& exception)
            {
                response.reset();
                response["CSeq"] = expectedCSeq;
                response["Agent"] = Primus::SoftwareVersion;
                response["Reason"] = "Missing CSeq";
                response.generateResponse(RTSP::BadRequest);

                throw Phoenix::RejectDatagram("Missing CSeq");
            }

            if (request.methodIs("ACTIVATE") == true)
            {
                try
                {
                    const std::string softwareVersion = request["Software-Version"];
                    const std::string activationCode = request["Activation-Code"];
                    const std::string vendorToken = request["Vendor-Token"];
                    const std::string deviceName = request["Device-Name"];
                    const std::string deviceModel = request["Device-Model"];

                    if (softwareVersion.length() == 0)
                    {
                        response.reset();
                        response["CSeq"] = expectedCSeq;
                        response["Agent"] = Primus::SoftwareVersion;
                        response["Reason"] = "Missing software version";
                        response.generateResponse(RTSP::BadRequest);

                        throw Phoenix::RejectDatagram("Missing software version");
                    }

                    if (activationCode.length() != Primus::ActivationCodeLength)
                    {
                        response.reset();
                        response["CSeq"] = expectedCSeq;
                        response["Agent"] = Primus::SoftwareVersion;
                        response["Reason"] = "Activation code in wrong format";
                        response.generateResponse(RTSP::NotAcceptable);

                        throw Phoenix::RejectDatagram("Activation code in wrong format");
                    }

                    if (vendorToken.length() != PostgreSQL::UUIDPlainLength)
                    {
                        response.reset();
                        response["CSeq"] = expectedCSeq;
                        response["Agent"] = Primus::SoftwareVersion;
                        response["Reason"] = "Bad ventor token";
                        response.generateResponse(RTSP::BadRequest);

                        throw Phoenix::RejectDatagram("Bad ventor token");
                    }

                    if (deviceName.length() == 0)
                    {
                        response.reset();
                        response["CSeq"] = expectedCSeq;
                        response["Agent"] = Primus::SoftwareVersion;
                        response["Reason"] = "Missing device name";
                        response.generateResponse(RTSP::BadRequest);

                        throw Phoenix::RejectDatagram("Missing device name");
                    }

                    if (deviceModel.length() == 0)
                    {
                        response.reset();
                        response["CSeq"] = expectedCSeq;
                        response["Agent"] = Primus::SoftwareVersion;
                        response["Reason"] = "Missing device model";
                        response.generateResponse(RTSP::BadRequest);

                        throw Phoenix::RejectDatagram("Missing device model");
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
                        connection->phoenix = &Database::Phoenixes::PhoenixById(phoenixId);

                        response.reset();
                        response["CSeq"] = expectedCSeq;
                        response["Agent"] = Primus::SoftwareVersion;
                        response["Walker-Token"] = connection->phoenix->token;
                        response.generateResponse(RTSP::OK);
                    }
                }
                catch (RTSP::StatementNotFound& exception)
                {
                    response.reset();
                    response["CSeq"] = expectedCSeq;
                    response["Agent"] = Primus::SoftwareVersion;
                    response["Reason"] = "Missing mandatory statements";
                    response.generateResponse(RTSP::BadRequest);

                    throw Phoenix::RejectDatagram("Missing statements");
                }
            }
            else if (request.methodIs("APNS") == true)
            {
                const std::string deviceToken = request["Device-Token"];

                char t[72];
                APNS::DeviceTokenFromString(t, deviceToken.c_str());
                APNS::DeviceTokenSymbolToBin(connection->phoenix->deviceToken, t);

                connection->phoenix->saveDeviceToken();

                response.reset();
                response["CSeq"] = expectedCSeq;
                response["Agent"] = Primus::SoftwareVersion;
                response.generateResponse(RTSP::Created);
            }
            else if (request.methodIs("LOGIN") == true)
            {
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

                        throw Phoenix::RejectDatagram("Bad phoenix token");
                    }

                    if (phoenixToken.length() != PostgreSQL::UUIDPlainLength)
                    {
                        response.reset();
                        response["CSeq"] = expectedCSeq;
                        response["Agent"] = Primus::SoftwareVersion;
                        response["Reason"] = "Walker token in wrong format";
                        response.generateResponse(RTSP::BadRequest);

                        throw Phoenix::RejectDatagram("Walker token in wrong format");
                    }

                    if (softwareVersion.length() == 0)
                    {
                        response.reset();
                        response["CSeq"] = expectedCSeq;
                        response["Agent"] = Primus::SoftwareVersion;
                        response["Reason"] = "Missing software version";
                        response.generateResponse(RTSP::BadRequest);

                        throw Phoenix::RejectDatagram("Missing software version");
                    }

                    connection->phoenix = &Database::Phoenixes::PhoenixByToken(phoenixToken);

                    connection->phoenix->setSoftwareVersion(softwareVersion);

                    response.reset();
                    response["CSeq"] = expectedCSeq;
                    response["Agent"] = Primus::SoftwareVersion;
                    response.generateResponse(RTSP::OK);
                }
                catch (RTSP::StatementNotFound& exception)
                {
                    response.reset();
                    response["CSeq"] = expectedCSeq;
                    response["Agent"] = Primus::SoftwareVersion;
                    response["Reason"] = "Missing mandatory statements";
                    response.generateResponse(RTSP::BadRequest);

                    throw Phoenix::RejectDatagram("Missing statements");
                }
            }
            else
            {
                response.reset();
                response["CSeq"] = expectedCSeq;
                response["Agent"] = Primus::SoftwareVersion;
                response.generateResponse(RTSP::MethodNotAllowed);

                throw Phoenix::RejectDatagram("Unknown method");
            }
        }
        catch (APNS::BrokenDeviceToken& exception)
        {
            ReportWarning("[Phoenix] Rejected: %s", exception.what());

            Primus::Debug::CommentPhoenixSession(debugSessionId, exception.what());

            sessionOK = false;
        }
        catch (Phoenix::RejectDatagram& exception)
        {
            ReportWarning("[Phoenix] Rejected: %s", exception.what());

            Primus::Debug::CommentPhoenixSession(debugSessionId, exception.what());

            sessionOK = false;
        }
        catch (std::exception& exception)
        {
            ReportWarning("[Phoenix] Rejected: %s", exception.what());

            Primus::Debug::CommentPhoenixSession(debugSessionId, exception.what());

            sessionOK = false;
        }

std::this_thread::sleep_for(std::chrono::seconds { 1 } );
        try
        {
            Communicator::Send(
                    connection->socket(),
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
                    connection->socket(),
                    configuration.network.phoenix.keepAlive);
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

    ReportInfo("[Phoenix] Thread is going to quit");

    delete connection;
}
