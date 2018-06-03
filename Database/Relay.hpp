#pragma once

// System definition files.
//
#include <cstdbool>
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
        bool                state;
        std::string         description;

    public:
        Relay(const unsigned long relayId);

        ~Relay();

        void
        setDescription(const std::string&);

        bool
        isOff(),
        isOn();

        void
        switchOff(),
        switchOn(),
        switchOver();
    };
};
