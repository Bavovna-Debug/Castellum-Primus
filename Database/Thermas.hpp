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
#include "Primus/Database/Therma.hpp"

namespace Database
{
    class Thermas
    {
    public:
        static unsigned long
        TotalNumber();

        static Database::Therma&
        SensorByIndex(const unsigned long);

        static Database::Therma&
        SensorById(const unsigned long);
    };

    void
    NoticeDSSensorTemperature(
        Toolkit::Timestamp& originTimestamp,
        const std::string&  sensorToken,
        const double        stampAsReal,
        const float         temperature);
};
