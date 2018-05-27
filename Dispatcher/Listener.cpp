// System definition files.
//
#include <chrono>
#include <thread>

// Common definition files.
//
#include "APNS/APNS.hpp"
#include "APNS/Notification.hpp"
#include "Toolkit/Report.h"

// Local definition files.
//
#include "Primus/Dispatcher/Listener.hpp"
#include "Primus/Dispatcher/Session.hpp"

Dispatcher::Listener::Listener(
    const IP::Family        family,
    const unsigned short    portNumber) :
Inherited(family, "", portNumber)
{
    this->thread = std::thread(&Dispatcher::Listener::ThreadHandler, this);
}

/**
 * @brief   Thread handler for service.
 */
void
Dispatcher::Listener::ThreadHandler(Dispatcher::Listener* listener)
{
    ReportNotice("[Dispatcher] Service thread has been started");

    // Endless loop. In case an error on socket layer occurs,
    // the socket recovery will start automatically
    // at the beginning of the loop.
    //
    for (;;)
    {
        try
        {
            listener->disconnect();
            listener->connect();

            for (;;)
            {
                Dispatcher::Session* session = new Dispatcher::Session { *listener };

                std::thread sessionThread
                {
                    &Dispatcher::Session::ThreadHandler,
                    session
                };

                sessionThread.detach();
            }
        }
        catch (Communicator::SocketError& exception)
        {
            ReportError("[Dispatcher] Exception: %s: errno=%d",
                    exception.what(),
                    exception.errorNumber);

            continue;
        }
        catch (std::exception& exception)
        {
            ReportError("[Dispatcher] Exception: %s",
                    exception.what());

            continue;
        }
    }

    // Make sure the socket is closed.
    //
    listener->disconnect();

    ReportWarning("[Dispatcher] Service thread is going to quit");
}
