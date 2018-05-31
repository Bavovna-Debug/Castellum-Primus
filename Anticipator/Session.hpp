#pragma once

// System definition files.
//
#include <stdexcept>

// Common definition files.
//
#include "Communicator/TCP.hpp"

// Local definition files.
//
#include "Primus/Database/Phoenix.hpp"

namespace Anticipator
{
    static const unsigned int BytesReceivePerStep = 2048;

    class Session : public TCP::Connection
    {
        typedef TCP::Connection Inherited;

    private:
        char* receiveBuffer;

    public:
        Database::Phoenix* phoenix;

    public:
        Session(TCP::Service&);

        ~Session();

        static void
        ThreadHandler(Anticipator::Session*);
    };

    class RejectDatagram : public std::runtime_error
    {
    public:
        RejectDatagram(const char* const reason) throw() :
        std::runtime_error(reason)
        { }
    };
};
