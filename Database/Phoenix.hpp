#pragma once

// System definition files.
//
#include <string>

// Common definition files.
//
#include "APNS/APNS.hpp"
#include "Toolkit/Times.hpp"

namespace Database
{
    class Phoenix
    {
    public:
        unsigned long       phoenixId;
        Toolkit::Timestamp* timestamp;
        std::string         phoenixToken;
        char                deviceToken[APNS::DeviceTokenLength];
        std::string         vendorToken;
        std::string         deviceName;
        std::string         deviceModel;
        std::string         softwareVersion;
        std::string         phoenixDescription;

    public:
        Phoenix(const unsigned long phoenixId);

        ~Phoenix();

        void
        saveDeviceToken();

        void
        setDescription(const std::string&);

        void
        setSoftwareVersion(const std::string&);

        static unsigned long
        RegisterPhoenixWithActivationCode(
            const std::string&  activationCode,
            const std::string&  vendorToken,
            const std::string&  deviceName,
            const std::string&  deviceModel,
            const std::string&  softwareVersion,
            const std::string&  phoenixDescription);
    };
};
