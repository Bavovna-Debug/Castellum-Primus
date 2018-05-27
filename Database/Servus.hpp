#pragma once

// System definition files.
//
#include <cstdbool>
#include <string>

// Common definition files.
//
#include "Toolkit/Times.hpp"

namespace Database
{
    class Servus
    {
    public:
        unsigned long       servusId;
        Toolkit::Timestamp* timestamp;
        std::string         servusToken;
        bool                enabled;
        bool                online;
        std::string         authenticator;
        std::string         servusDescription;

    public:
        Servus(const unsigned long servusId);

        ~Servus();

        void
        setOnline();

        void
        setOffline();

        void
        toggleEnabledFlag();

        void
        setDescription(const std::string&);

        static unsigned long
        DefineServus(const std::string& servusDescription);
    };
};
