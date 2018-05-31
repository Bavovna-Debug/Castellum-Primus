#pragma once

// System definition files.
//
#include <string>

// Common definition files.
//
#include "Toolkit/Times.hpp"

namespace Database
{
    class Relay
    {
    public:
        Toolkit::Timestamp* timestamp;
        unsigned long       relayId;
        std::string         token;
        unsigned long       servusId;
        unsigned int        gpioPinNumber;
        std::string         description;

    public:
        Relay(const unsigned long relayId);

        ~Relay();

        void
        setDescription(const std::string&);
    };
};
