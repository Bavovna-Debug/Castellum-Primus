#pragma once

// System definition files.
//
#include <string>

// Common definition files.
//
#include "Toolkit/Times.hpp"

namespace Database
{
    class DHTSensor
    {
    public:
        Toolkit::Timestamp* timestamp;
        unsigned long       sensorId;
        std::string         token;
        unsigned long       servusId;
        std::string         gpioPinNumber;
        float               humidityEdge;
        float               temperatureEdge;
        std::string         title;

    public:
        DHTSensor(const unsigned long sensorId);

        ~DHTSensor();

        float
        lastKnownHumidity(),
        lowestKnownHumidity(),
        highestKnownHumidity();

        float
        lastKnownTemperature(),
        lowestKnownTemperature(),
        highestKnownTemperature();
    };
};
