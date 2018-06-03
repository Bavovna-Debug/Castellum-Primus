#pragma once

// System definition files.
//
#include <string>

// Local definition files.
//
#include "Primus/Database/Phoenix.hpp"

namespace Database
{
    class Phoenixes
    {
    public:
        static unsigned long
        TotalNumber();

        static Database::Phoenix&
        PhoenixByIndex(const unsigned long);

        static Database::Phoenix&
        PhoenixByToken(const std::string&);

        static Database::Phoenix&
        PhoenixById(const unsigned long);

        static void
        RemovePhoenixById(const unsigned long);
    };
};
