// System definition files.
//
#include <unistd.h>
#include <cstdbool>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>
#include <thread>

// Common definition files.
//
#include "Communicator/TCP.hpp"
#include "Toolkit/Report.h"

// Local definition files.
//
#include "Primus/Anticipator/Listener.hpp"
#include "Primus/Anticipator/Session.hpp"

Anticipator::Listener::Listener(
    const IP::Family        family,
    const unsigned short    portNumber) :
Inherited(family, "", portNumber)
{
    this->thread = std::thread(&Anticipator::Listener::ThreadHandler, this);
}

/**
 * @brief   Thread handler for service.
 */
void
Anticipator::Listener::ThreadHandler(Anticipator::Listener* listener)
{
    ReportNotice("[Anticipator] Service thread has been started");

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
                Anticipator::Session* session = new Anticipator::Session { *listener };

                std::thread sessionThread
                {
                    &Anticipator::Session::ThreadHandler,
                    session
                };

                sessionThread.detach();
            }
        }
        catch (Communicator::SocketError& exception)
        {
            ReportError("[Anticipator] Exception: %s: errno=%d",
                    exception.what(),
                    exception.errorNumber);

            continue;
        }
        catch (std::exception& exception)
        {
            ReportError("[Anticipator] Exception: %s",
                    exception.what());

            continue;
        }
    }

    // Make sure the socket is closed.
    //
    listener->disconnect();

    ReportWarning("[Anticipator] Service thread is going to quit");
}
