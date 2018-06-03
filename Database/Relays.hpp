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
#include "Primus/Database/Relay.hpp"

namespace Database
{
    class Relays
    {
    public:
        static unsigned long
        TotalNumber();

        static Database::Relay&
        RelayByIndex(const unsigned long);

        static Database::Relay&
        RelayById(const unsigned long);
    };

    class RelayNotFound : public std::runtime_error
    {
    public:
        RelayNotFound() throw() :
        std::runtime_error("")
        { }
    };
};
