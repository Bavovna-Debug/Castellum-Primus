#pragma once

// System definition files.
//
#include <stdexcept>
#include <thread>

// Common definition files.
//
#include "Communicator/TCP.hpp"

// Local definition files.
//
#include "Primus/Database/Phoenix.hpp"

namespace Phoenix
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
        ThreadHandler(Listener*);
    };

    class Connection : public TCP::Connection
    {
        typedef TCP::Connection Inherited;

    private:
        char* receiveBuffer;

    public:
        Database::Phoenix* phoenix;

    public:
        Connection(TCP::Service&);

        ~Connection();

        static void
        ThreadHandler(Connection*);
    };

    class RejectDatagram : public std::runtime_error
    {
    public:
        RejectDatagram(const char* const reason) throw() :
        std::runtime_error(reason)
        { }
    };
};
