#pragma once

// System definition files.
//
#include <string>

// Common definition files.
//
#include "PostgreSQL/PostgreSQL.hpp"

// Local definition files.
//
#include "Primus/Database/Servus.hpp"

namespace Database
{
    class Servuses
    {
    public:
        static unsigned long
        TotalNumber();

        static Database::Servus&
        ServusByAuthenticator(const std::string&);

        static Database::Servus&
        ServusByIndex(const unsigned long);

        static Database::Servus&
        ServusById(const unsigned long);
    };

    class ServusNotFound : public std::runtime_error
    {
    public:
        ServusNotFound() throw() :
        std::runtime_error("")
        { }
    };
};
