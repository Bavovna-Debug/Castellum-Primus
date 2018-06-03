#pragma once

// Local definition files.
//
#include "Primus/Anticipator/Listener.hpp"

namespace Anticipator
{
    class Service
    {
    private:
        Anticipator::Listener* listenerIPv4;
        Anticipator::Listener* listenerIPv6;

    public:
        static Anticipator::Service&
        InitInstance();

        static Anticipator::Service&
        SharedInstance();

        Service();
    };

    const std::string
    ServerKey();
};
