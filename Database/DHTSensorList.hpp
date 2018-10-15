#pragma once

// System definition files.
//
#include <string>

// Common definition files.
//
#include "PostgreSQL/PostgreSQL.hpp"
#include "Toolkit/Times.hpp"

// Local definition files.
//
#include "Primus/Database/DHTSensor.hpp"

namespace Database
{
    class DHTSensorList
    {
    public:
        static unsigned long
        TotalNumber();

        static Database::DHTSensor&
        SensorByIndex(const unsigned long);

        static Database::DHTSensor&
        SensorById(const unsigned long);
    };

    void
    NoticeDHTSensorHumidity(
        Toolkit::Timestamp& originTimestamp,
        const std::string&  sensorToken,
        const double        stampAsReal,
        const float         humidity);

    void
    NoticeDHTSensorTemperature(
        Toolkit::Timestamp& originTimestamp,
        const std::string&  sensorToken,
        const double        stampAsReal,
        const float         temperature);
};
