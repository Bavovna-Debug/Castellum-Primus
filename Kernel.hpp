#pragma once

// Common definition files.
//
#include "HTTP/Service.hpp"
#include "Toolkit/Workspace.hpp"
#include "Toolkit/Times.hpp"

namespace Workspace
{
    static const std::string RootPath = "/opt/castellum/";

    class Kernel : public Application
    {
        typedef Application Inherited;

    public:
        /**
         * UNIX version 6 timestamp representing the time the Primus has been started
         * (in seconds since 1970-01-01 00:00:00 GMT).
         */
        Toolkit::Timestamp*     timestampOfStart;

        HTTP::Service*          http;

    public:
        static Workspace::Kernel&
        InitInstance();

        static Workspace::Kernel&
        SharedInstance();

        Kernel();

    protected:
        void
        kernelInit(),
        kernelExec(),
        kernelWait(),
        kernelDone();
    };
};
