#pragma once

// System definition files.
//
#include <thread>

// Common definition files.
//
#include "Communicator/TCP.hpp"

// Local definition files.
//
#include "Primus/Database/Phoenix.hpp"

namespace Anticipator
{
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
        ThreadHandler(Anticipator::Listener*);
    };
};
