#pragma once

// System definition files.
//
#include <stdexcept>

// Common definition files.
//
#include "Communicator/TCP.hpp"
#include "RTSP/RTSP.hpp"

// Local definition files.
//
#include "Primus/Database/Phoenix.hpp"

namespace Anticipator
{
    static const unsigned int BytesReceivePerStep = 64 * 1024;

    class Session : public TCP::Connection
    {
        typedef TCP::Connection Inherited;

    private:
        char*               receiveBuffer;

    public:
        RTSP::Datagram      request;
        RTSP::Datagram      response;
        unsigned int        expectedCSeq;
        Database::Phoenix*  phoenix;

    public:
        Session(TCP::Service&);

        ~Session();

        static void
        ThreadHandler(Anticipator::Session*);

        void
        handleActivate(),
        handleAPNS(),
        handleLogin(),
        handleServusList(),
        handleFabulaList(),
        handleUnknown(),
        loginRequired();
    };

    class RejectDatagram : public std::runtime_error
    {
    public:
        RejectDatagram(const char* const reason) throw() :
        std::runtime_error(reason)
        { }
    };
};
