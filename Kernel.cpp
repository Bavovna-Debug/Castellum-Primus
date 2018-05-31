// System definition files.
//
#include <unistd.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

// Common definition files.
//
#include "APNS/APNS.hpp"
#include "Communicator/TCP.hpp"
#include "HTTP/Service.hpp"
#include "Toolkit/Report.h"
#include "Toolkit/Signals.hpp"
#include "Toolkit/Times.hpp"

// Local definition files.
//
#include "Primus/Configuration.hpp"
#include "Primus/Kernel.hpp"
#include "Primus/Database/Database.hpp"
#include "Primus/Database/Debug.hpp"
#include "Primus/Dispatcher/Listener.hpp"
#include "Primus/Dispatcher/Notificator.hpp"
#include "Primus/Phoenix/Anticipator.hpp"
#include "Primus/WWW/Home.hpp"
#include "Quasar/WWW/SessionManager.hpp"

static void
OwnSignalHandler(int signalNumber);

static Workspace::Kernel* instance = NULL;

Workspace::Kernel&
Workspace::Kernel::InitInstance()
{
    if (instance != NULL)
        throw std::runtime_error("Kernel already initialized");

    instance = new Workspace::Kernel();

    return *instance;
}

Workspace::Kernel&
Workspace::Kernel::SharedInstance()
{
    if (instance == NULL)
        throw std::runtime_error("Kernel not initialized");

    return *instance;
}

Workspace::Kernel::Kernel() :
Inherited()
{
    this->timestampOfStart = new Toolkit::Timestamp();
}

/**
 * @brief   1st part of application kernel - initialize all resources.
 *
 * @return  0 upon successful completion.
 * @return  Error code (negative value), in case of error.
 */
void
Workspace::Kernel::kernelInit()
{
    try
    {
        Primus::Configuration::InitInstance("/opt/castellum/primus.conf");
        Primus::Database::InitInstance(Primus::Database::Default);
        Primus::Database::InitInstance(Primus::Database::Fabulatorium);
        Primus::Database::InitInstance(Primus::Database::Notificator);
        Primus::Database::InitInstance(Primus::Database::WWW);
        Primus::Debug::InitInstance();
        Phoenix::Anticipator::InitInstance();
        Dispatcher::Notificator::InitInstance();
        APNS::Service::InitInstance();
        WWW::SessionManager::InitInstance();
    }
    catch (std::exception& exception)
    {
        ReportWarning("[Workspace] Exception: %s", exception.what());
    }

    Primus::Configuration& configuration = Primus::Configuration::SharedInstance();

    APNS::Service& apns = APNS::Service::SharedInstance();
    apns.setDelayAfterWakeup(configuration.apns.delayAfterWakeup);
    apns.setDelayBetweenFrames(configuration.apns.delayBetweenFrames);
    apns.setDelayAfterCompletion(configuration.apns.delayAfterCompletion);
    apns.setPauseBeforeReconnect(configuration.apns.pauseBeforeReconnect);
    apns.setup(configuration.apns.sandbox, *configuration.apns.certificate);

    // Start HTTP service.
    //
    this->http = new HTTP::Service(new WWW::Site(),
            IP::IPv4,
            "",
            configuration.http.portNumber,
            0x9000,
            0xAA00,
            this->timestampOfStart->seconds());

    new Dispatcher::Listener(
        IP::IPv4,
        configuration.servus.portNumberIPv4);

    new Dispatcher::Listener(
        IP::IPv6,
        configuration.servus.portNumberIPv6);

    Toolkit::SetSignalCaptureOn(SIGINT, OwnSignalHandler);
    Toolkit::SetSignalCaptureOn(SIGTERM, OwnSignalHandler);
}

/**
 * @brief   2nd part of application kernel - start all subcomponents.
 *
 * @return  0 upon successful completion.
 * @return  Error code (negative value), in case of error.
 */
void
Workspace::Kernel::kernelExec()
{
    Dispatcher::Notificator& notificator = Dispatcher::Notificator::SharedInstance();
    notificator.startService();

    try
    {
        this->http->startService();
    }
    catch (...)
    {
        ReportError("[Workspace] Error has occurred starting services");
    }
}

/**
 * @brief   3rd part of application kernel - waiting for completion of services.
 *
 * @return  0 upon successful completion.
 * @return  Error code (negative value), in case of error.
 */
void
Workspace::Kernel::kernelWait()
{
    try
    {
        this->http->waitForService();
    }
    catch (...)
    {
        ReportError("[Workspace] Error has occurred waiting for services");
    }
}

/**
 * @brief   4th part of application kernel - release all resources.
 *
 * @return  0 upon successful completion.
 * @return  Error code (negative value), in case of error.
 */
void
Workspace::Kernel::kernelDone()
{
    Toolkit::SetSignalCaptureOff(SIGINT);
    Toolkit::SetSignalCaptureOff(SIGTERM);

    try
    {
        this->http->stopService();
    }
    catch (...)
    {
        ReportError("[Workspace] Error has occurred trying to stop services");
    }
}

/**
 * @brief   POSIX signal handler.
 *
 * Handle POSIX process signals to let procurator release all resources.
 *
 * @param   signalNumber    POSIX signal number.
 */
static void
OwnSignalHandler(int signalNumber)
{
    ReportNotice("[Workspace] Received signal to quit: signal=%d", signalNumber);

    // Raise signal again to let it be handled by default signal handler.
    //
    raise(SIGHUP);
}
