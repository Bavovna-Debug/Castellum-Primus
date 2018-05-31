#pragma once

// System definition files.
//
#include <string>

// Common definition files.
//
#include "Toolkit/Times.hpp"

namespace Database
{
    class Therma
    {
    public:
        Toolkit::Timestamp* timestamp;
        unsigned long       thermaId;
        std::string         token;
        unsigned long       servusId;
        std::string         gpioDeviceNumber;
        float               edge;
        std::string         description;

    public:
        Therma(const unsigned long thermaId);

        ~Therma();

        void
        setDescription(const std::string&);

        float
        lastKnownTemperature(),
        lowestKnownTemperature(),
        highestKnownTemperature();
    };
};
