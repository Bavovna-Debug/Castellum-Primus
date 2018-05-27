#pragma once

// Local definition files.
//
#include "Primus/Dispatcher/Fabula.hpp"

namespace Dispatcher
{
    class Fabulas
    {
    public:
        unsigned long
        totalNumber();

        Fabula&
        fabulaByIndex(const unsigned long);

        Fabula&
        fabulaById(const unsigned long);
    };
};
