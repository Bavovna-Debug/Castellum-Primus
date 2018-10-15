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
        float               temperatureEdge;
        std::string         title;

    public:
        Therma(const unsigned long thermaId);

        ~Therma();

        void
        setTitle(const std::string&);

        float
        lastKnownTemperature(),
        lowestKnownTemperature(),
        highestKnownTemperature();

        std::string
        diagramAsJava();
    };
};
