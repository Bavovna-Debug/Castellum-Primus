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
        Toolkit::Timestamp* timestamp;
        unsigned long       servusId;
        std::string         token;
        bool                enabled;
        bool                online;
        std::string         authenticator;
        std::string         description;

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
        DefineServus(const std::string& description);
    };
};
