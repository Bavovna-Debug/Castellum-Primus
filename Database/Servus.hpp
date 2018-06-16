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
        Toolkit::Timestamp* runningSince;
        std::string         authenticator;
        std::string         title;

    public:
        Servus(const unsigned long servusId);

        ~Servus();

        std::string
        configurationJSON();

        void
        toggleEnabledFlag(),
        setOnline(),
        setOffline(),
        setRunningSince(Toolkit::Timestamp&);

        void
        setTitle(const std::string&);
    };
};
