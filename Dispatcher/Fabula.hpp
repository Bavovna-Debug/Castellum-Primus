#pragma once

// System definition files.
//
#include <string>

// Common definition files.
//
#include "Toolkit/Times.hpp"

namespace Dispatcher
{
    class Fabula
    {
    public:
        unsigned long       fabulaId;
        std::string         fabulaToken;
        Toolkit::Timestamp* originTimestamp;
        unsigned long       servusId;
        std::string         originatorLabel;
        unsigned short      severityLevel;
        std::string         message;

    public:
        Fabula(const unsigned long fabulaId);

        ~Fabula();

        static std::string
        Enqueue(
            Toolkit::Timestamp&     originTimestamp,
            const unsigned long     servusId,
            const std::string&      originatorLabel,
            const unsigned short    severityLevel,
            const std::string&      message);
    };
};
