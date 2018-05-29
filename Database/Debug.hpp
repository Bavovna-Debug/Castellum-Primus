#pragma once

// System definition files.
//
#include <mutex>
#include <string>

// Common definition files.
//
#include "PostgreSQL/Exception.hpp"
#include "PostgreSQL/PostgreSQL.hpp"
#include "RTSP/RTSP.hpp"

namespace Primus
{
    class Debug
    {
    public:
        std::mutex lock;

        PostgreSQL::Connection* connection;

    public:
        static Primus::Debug&
        InitInstance();

        static Primus::Debug&
        SharedInstance();

        Debug();

        ~Debug();

        void
        connect(),
        disconnect(),
        recover(PostgreSQL::OperatorIntervention&);

        static unsigned long
        BeginServusSession(const std::string& servusIP);

        static void
        CloseServusSession(const unsigned long sessionId);

        static void
        CommentServusSession(
            const unsigned long sessionId,
            const std::string&  comment);

        static void
        ReportServusRTSP(
            const unsigned long sessionId,
            RTSP::Datagram&     request,
            RTSP::Datagram&     response);

        static unsigned long
        BeginPhoenixSession(const std::string& phoenixIP);

        static void
        ClosePhoenixSession(const unsigned long sessionId);

        static void
        CommentPhoenixSession(
            const unsigned long sessionId,
            const std::string&  comment);

        static void
        ReportPhoenixRTSP(
            const unsigned long sessionId,
            RTSP::Datagram&     request,
            RTSP::Datagram&     response);
    };
};
