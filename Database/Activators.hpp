#pragma once

// Local definition files.
//
#include "Primus/Database/Activator.hpp"

namespace Database
{
    class Activators
    {
    public:
        static unsigned long
        TotalNumber();

        static Database::Activator&
        ActivatorByIndex(const unsigned long);

        static Database::Activator&
        ActivatorById(const unsigned long);
    };
};
