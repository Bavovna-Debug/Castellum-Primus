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
        Toolkit::Timestamp* timestamp;
        unsigned long       activatorId;
        std::string         activatorToken;
        unsigned long       phoenixId;
        std::string         activationCode;
        std::string         title;

    public:
        Activator(const unsigned long activatorId);

        ~Activator();

        void
        setActivationCode(const std::string&);

        void
        setTitle(const std::string&);

        static unsigned long
        DefineActivator(
            const std::string&  activationCode,
            const std::string&  title);
    };
};
