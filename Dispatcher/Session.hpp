#pragma once

// System definition files.
//
#include <stdexcept>

// Common definition files.
//
#include "Communicator/TCP.hpp"

// Local definition files.
//
#include "Primus/Database/Servus.hpp"

namespace Dispatcher
{
    class Session : public TCP::Connection
    {
        typedef TCP::Connection Inherited;

    private:
        char*               receiveBuffer;
        Database::Servus*   servus;

    public:
        Session(TCP::Service&);

        ~Session();

        static void
        ThreadHandler(Session*);
    };

    void transformToken(char*, char* const);

    void xmit(char*);

    class RejectDatagram : public std::runtime_error
    {
    public:
        RejectDatagram(const char* const reason) throw() :
        std::runtime_error(reason)
        { }
    };
};
