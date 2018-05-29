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
        ThermaByIndex(const unsigned long);

        static Database::Therma&
        ThermaById(const unsigned long);
    };

    void
    NoticeTemperature(
        Toolkit::Timestamp& originTimestamp,
        const std::string&  sensorToken,
        const double        stampAsReal,
        const float         temperature);

    class ThermaNotFound : public std::runtime_error
    {
    public:
        ThermaNotFound() throw() :
        std::runtime_error("")
        { }
    };
};
