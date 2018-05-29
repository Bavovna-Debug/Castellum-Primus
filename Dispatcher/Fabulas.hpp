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

        Dispatcher::Fabula&
        fabulaByIndex(const unsigned long);

        Dispatcher::Fabula&
        fabulaById(const unsigned long);
    };
};
