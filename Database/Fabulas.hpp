#pragma once

// Local definition files.
//
#include "Primus/Database/Fabula.hpp"

namespace Database
{
    class Fabulas
    {
    public:
        unsigned long
        totalNumber();

        Database::Fabula&
        fabulaByIndex(const unsigned long);

        Database::Fabula&
        fabulaById(const unsigned long);

        static const std::string
        FabulasAsXML(
            const std::string&  lastKnownFabulaToken,
            const unsigned int  maximalFabulasPerXML);
    };
};
