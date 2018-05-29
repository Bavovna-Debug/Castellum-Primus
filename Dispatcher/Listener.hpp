#pragma once

// System definition files.
//
#include <thread>

// Common definition files.
//
#include "Communicator/TCP.hpp"

namespace Dispatcher
{
    static const unsigned int BytesReceivePerStep = 2048;

    class Listener : public TCP::Service
    {
        typedef TCP::Service Inherited;

    private:
        /**
         * Thread handler of service thread.
         */
        std::thread thread;

    public:
        Listener(
            const IP::Family        family,
            const unsigned short    portNumber);

    private:
        static void
        ThreadHandler(Dispatcher::Listener*);
    };
};
