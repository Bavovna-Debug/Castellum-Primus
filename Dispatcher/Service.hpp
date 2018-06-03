#pragma once

// Local definition files.
//
#include "Primus/Dispatcher/Listener.hpp"

namespace Dispatcher
{
    class Service
    {
    private:
        Dispatcher::Listener* listenerIPv4;
        Dispatcher::Listener* listenerIPv6;

    public:
        static Dispatcher::Service&
        InitInstance();

        static Dispatcher::Service&
        SharedInstance();

        Service();
    };
};
