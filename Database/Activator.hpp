#pragma once

// System definition files.
//
#include <string>

// Common definition files.
//
#include "Toolkit/Times.hpp"

namespace Database
{
    class Activator
    {
    public:
        unsigned long       activatorId;
        Toolkit::Timestamp* timestamp;
        std::string         activatorToken;
        unsigned long       phoenixId;
        std::string         activationCode;
        std::string         activatorDescription;

    public:
        Activator(const unsigned long activatorId);

        ~Activator();

        void
        setActivationCode(const std::string&);

        void
        setDescription(const std::string&);

        static unsigned long
        DefineActivator(
            const std::string&  activationCode,
            const std::string&  description);
    };
};
