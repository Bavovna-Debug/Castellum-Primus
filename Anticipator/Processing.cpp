// System definition files.
//
#include <cstdbool>
#include <cstring>
#include <string>
#include <thread>

// Common definition files.
//
#include "APNS/APNS.hpp"
#include "PostgreSQL/PostgreSQL.hpp"
#include "RTSP/RTSP.hpp"
#include "Toolkit/Report.h"

// Local definition files.
//
#include "Primus/Configuration.hpp"
#include "Primus/Anticipator/Session.hpp"
#include "Primus/Database/Fabulas.hpp"
#include "Primus/Database/Phoenix.hpp"
#include "Primus/Database/Phoenixes.hpp"
#include "Primus/Database/Servus.hpp"
#include "Primus/Database/Servuses.hpp"

void
Anticipator::Session::handleActivate()
{
    Primus::Configuration& configuration = Primus::Configuration::SharedInstance();

    std::this_thread::sleep_for(std::chrono::milliseconds
            { configuration.phoenix.delayResponseForActivate } );

    try
    {
        const std::string softwareVersion = this->request["Software-Version"];
        const std::string activationCode = this->request["Activation-Code"];
        const std::string vendorToken = this->request["Vendor-Token"];
        const std::string deviceName = this->request["Device-Name"];
        const std::string deviceModel = this->request["Device-Model"];

        if (softwareVersion.length() == 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds
                    { configuration.phoenix.delayResponseForRejected } );

            this->response.reset();
            this->response["CSeq"] = this->expectedCSeq;
            this->response["Agent"] = Primus::SoftwareVersion;
            this->response["Reason"] = "Missing software version";
            this->response.generateResponse(RTSP::BadRequest);

            throw Anticipator::RejectDatagram("Missing software version");
        }

        if (activationCode.length() != Primus::ActivationCodeLength)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds
                    { configuration.phoenix.delayResponseForRejected } );

            this->response.reset();
            this->response["CSeq"] = this->expectedCSeq;
            this->response["Agent"] = Primus::SoftwareVersion;
            this->response["Reason"] = "Activation code in wrong format";
            this->response.generateResponse(RTSP::NotAcceptable);

            throw Anticipator::RejectDatagram("Activation code in wrong format");
        }

        if (vendorToken.length() != PostgreSQL::UUIDPlainLength)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds
                    { configuration.phoenix.delayResponseForRejected } );

            this->response.reset();
            this->response["CSeq"] = this->expectedCSeq;
            this->response["Agent"] = Primus::SoftwareVersion;
            this->response["Reason"] = "Bad ventor token";
            this->response.generateResponse(RTSP::BadRequest);

            throw Anticipator::RejectDatagram("Bad ventor token");
        }

        if (deviceName.length() == 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds
                    { configuration.phoenix.delayResponseForRejected } );

            this->response.reset();
            this->response["CSeq"] = this->expectedCSeq;
            this->response["Agent"] = Primus::SoftwareVersion;
            this->response["Reason"] = "Missing device name";
            this->response.generateResponse(RTSP::BadRequest);

            throw Anticipator::RejectDatagram("Missing device name");
        }

        if (deviceModel.length() == 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds
                    { configuration.phoenix.delayResponseForRejected } );

            this->response.reset();
            this->response["CSeq"] = this->expectedCSeq;
            this->response["Agent"] = Primus::SoftwareVersion;
            this->response["Reason"] = "Missing device model";
            this->response.generateResponse(RTSP::BadRequest);

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
            this->response.reset();
            this->response["CSeq"] = this->expectedCSeq;
            this->response["Agent"] = Primus::SoftwareVersion;
            this->response.generateResponse(RTSP::NotFound);
        }
        else
        {
            this->phoenix = &Database::Phoenixes::PhoenixById(phoenixId);

            this->response.reset();
            this->response["CSeq"] = this->expectedCSeq;
            this->response["Agent"] = Primus::SoftwareVersion;
            this->response["Walker-Token"] = this->phoenix->token;
            this->response.generateResponse(RTSP::OK);
        }
    }
    catch (RTSP::StatementNotFound& exception)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds
                { configuration.phoenix.delayResponseForRejected } );

        this->response.reset();
        this->response["CSeq"] = this->expectedCSeq;
        this->response["Agent"] = Primus::SoftwareVersion;
        this->response["Reason"] = "Missing mandatory statements";
        this->response.generateResponse(RTSP::BadRequest);

        throw Anticipator::RejectDatagram("Missing statements");
    }
}

void
Anticipator::Session::handleAPNS()
{
    if (this->phoenix == nullptr)
    {
        this->loginRequired();

        return;
    }

    const std::string deviceToken = this->request["Device-Token"];

    char t[72];
    APNS::DeviceTokenFromString(t, deviceToken.c_str());
    APNS::DeviceTokenSymbolToBin(this->phoenix->deviceToken, t);

    this->phoenix->saveDeviceToken();

    this->response.reset();
    this->response["CSeq"] = this->expectedCSeq;
    this->response["Agent"] = Primus::SoftwareVersion;
    this->response.generateResponse(RTSP::Created);
}

void
Anticipator::Session::handleLogin()
{
    Primus::Configuration& configuration = Primus::Configuration::SharedInstance();

    std::this_thread::sleep_for(std::chrono::milliseconds
            { configuration.phoenix.delayResponseForLogin } );

    try
    {
        const std::string phoenixToken = this->request["Walker-Token"];
        const std::string softwareVersion = this->request["Software-Version"];

        if (phoenixToken.length() == 0)
        {
            this->response.reset();
            this->response["CSeq"] = this->expectedCSeq;
            this->response["Agent"] = Primus::SoftwareVersion;
            this->response["Reason"] = "Bad phoenix token";
            this->response.generateResponse(RTSP::BadRequest);

            throw Anticipator::RejectDatagram("Bad phoenix token");
        }

        if (phoenixToken.length() != PostgreSQL::UUIDPlainLength)
        {
            this->response.reset();
            this->response["CSeq"] = this->expectedCSeq;
            this->response["Agent"] = Primus::SoftwareVersion;
            this->response["Reason"] = "Phoenix token in wrong format";
            this->response.generateResponse(RTSP::BadRequest);

            throw Anticipator::RejectDatagram("Phoenix token in wrong format");
        }

        if (softwareVersion.length() == 0)
        {
            this->response.reset();
            this->response["CSeq"] = this->expectedCSeq;
            this->response["Agent"] = Primus::SoftwareVersion;
            this->response["Reason"] = "Missing software version";
            this->response.generateResponse(RTSP::BadRequest);

            throw Anticipator::RejectDatagram("Missing software version");
        }

        this->phoenix = &Database::Phoenixes::PhoenixByToken(phoenixToken);

        this->phoenix->setSoftwareVersion(softwareVersion);

        this->response.reset();
        this->response["CSeq"] = this->expectedCSeq;
        this->response["Agent"] = Primus::SoftwareVersion;
        this->response.generateResponse(RTSP::OK);
    }
    catch (RTSP::StatementNotFound& exception)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds
                { configuration.phoenix.delayResponseForRejected } );

        this->response.reset();
        this->response["CSeq"] = this->expectedCSeq;
        this->response["Agent"] = Primus::SoftwareVersion;
        this->response["Reason"] = "Missing mandatory statements";
        this->response.generateResponse(RTSP::BadRequest);

        throw Anticipator::RejectDatagram("Missing statements");
    }
}

void
Anticipator::Session::handleServusList()
{
    if (this->phoenix == nullptr)
    {
        this->loginRequired();

        return;
    }

    this->response.reset();
    this->response["CSeq"] = this->expectedCSeq;
    this->response["Agent"] = Primus::SoftwareVersion;

    const std::string payload =
            Database::Servuses::ServusesAsXML();

    this->response.generateResponse(RTSP::OK, payload);
}

void
Anticipator::Session::handleFabulaList()
{
    if (this->phoenix == nullptr)
    {
        this->loginRequired();

        return;
    }

#if 0
    try
    {
        const std::string lastKnownFabulaToken = this->request["Last-Known"];

        if (lastKnownFabulaToken.length() != PostgreSQL::UUIDPlainLength)
        {
            this->response.reset();
            this->response["CSeq"] = this->expectedCSeq;
            this->response["Agent"] = Primus::SoftwareVersion;
            this->response["Reason"] = "Fabula token in wrong format";
            this->response.generateResponse(RTSP::BadRequest);

            throw Anticipator::RejectDatagram("Fabula token in wrong format");
        }

        // Last known fabula has been specified by phoenix.

        this->response.reset();
        this->response["CSeq"] = this->expectedCSeq;
        this->response["Agent"] = Primus::SoftwareVersion;

        const std::string payload =
                Database::Fabulas::FabulasAsXML(
                        lastKnownFabulaToken,
                        Primus::MaximalNumberOfFabulasPerXML);

        this->response.generateResponse(RTSP::OK, payload);
    }
    catch (RTSP::StatementNotFound& exception)
    {
        // Last known fabula has not been specified by phoenix.
        // This might be either a fresh activated phoenix or a phoenix
        // that has flushed its local database.

        this->response.reset();
        this->response["CSeq"] = this->expectedCSeq;
        this->response["Agent"] = Primus::SoftwareVersion;

        const std::string payload =
                Database::Fabulas::FabulasAsXML(
                        "",
                        Primus::MaximalNumberOfFabulasPerXML);

        this->response.generateResponse(RTSP::OK, payload);
    }
#endif
}

void
Anticipator::Session::handleUnknown()
{
    Primus::Configuration& configuration = Primus::Configuration::SharedInstance();

    std::this_thread::sleep_for(std::chrono::milliseconds
            { configuration.phoenix.delayResponseForRejected } );

    this->response.reset();
    this->response["CSeq"] = this->expectedCSeq;
    this->response["Agent"] = Primus::SoftwareVersion;
    this->response.generateResponse(RTSP::MethodNotAllowed);

    throw Anticipator::RejectDatagram("Unknown method");
}

void
Anticipator::Session::loginRequired()
{
    ReportWarning("[Anticipator] Phoenix calls %s before having authenticated itself",
            this->request.method().c_str());

    this->response.reset();
    this->response["CSeq"] = this->expectedCSeq;
    this->response["Agent"] = Primus::SoftwareVersion;
    this->response["Reason"] = "Session not authenticated";
    this->response.generateResponse(RTSP::Forbidden);
}
