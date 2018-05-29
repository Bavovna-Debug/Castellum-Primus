#pragma once

// Local definition files.
//
#include "Primus/Phoenix/Listener.hpp"

namespace Phoenix
{
    class Anticipator
    {
    private:
        Phoenix::Listener* listenerIPv4;
        Phoenix::Listener* listenerIPv6;

    public:
        static Phoenix::Anticipator&
        InitInstance();

        static Phoenix::Anticipator&
        SharedInstance();

        Anticipator();
    };
};
