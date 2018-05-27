// System definition files.
//
#include <chrono>
#include <condition_variable>
#include <thread>

// Common definition files.
//
#include "APNS/APNS.hpp"
#include "APNS/Notification.hpp"
#include "PostgreSQL/PostgreSQL.hpp"
#include "Toolkit/Report.h"

// Local definition files.
//
#include "Primus/Database/Database.hpp"
#include "Primus/Database/Queries/Notificator.h"
#include "Primus/Dispatcher/Notificator.hpp"

static Dispatcher::Notificator* instance = NULL;

Dispatcher::Notificator&
Dispatcher::Notificator::InitInstance()
{
    if (instance != NULL)
        throw std::runtime_error("[Notificator] Already initialized");

    instance = new Dispatcher::Notificator();

    return *instance;
}

Dispatcher::Notificator&
Dispatcher::Notificator::SharedInstance()
{
    if (instance == NULL)
        throw std::runtime_error("[Notificator] Not initialized");

    return *instance;
}

void
Dispatcher::Notificator::startService()
{
    this->thread = std::thread(&Dispatcher::Notificator::ThreadHandler, this);
}

void
Dispatcher::Notificator::triggerProcessing()
{
    ReportDebug("[Notificator] Triggered");

    std::unique_lock<std::mutex> queueLock { this->trigger.lock, std::defer_lock };

    queueLock.lock();

    this->trigger.condition.notify_one();

    queueLock.unlock();
}

std::cv_status
Dispatcher::Notificator::waitForTriggerWithTimeout(const std::chrono::seconds duration)
{
    std::unique_lock<std::mutex> queueLock { this->trigger.lock, std::defer_lock };

    std::cv_status status;

    queueLock.lock();

    if (this->numberOfPendingNotifications() == 0)
    {
        status = this->trigger.condition.wait_for(queueLock, duration);
    }
    else
    {
        status = std::cv_status::no_timeout;
    }

    queueLock.unlock();

    return status;
}

unsigned long
Dispatcher::Notificator::numberOfPendingNotifications()
{
    unsigned long numberOfNotifications;

    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Notificator);

    try
    {
        PostgreSQL::Query query(*database.connection);

        query.execute(QueryNumberOfPendingNotifications);

        query.assertNumberOfRows(1);
        query.assertNumberOfColumns(1);
        query.assertColumnOfType(0, PostgreSQL::INT8OID);

        numberOfNotifications = query.popBIGINT();
    }
    catch (PostgreSQL::UnexpectedQueryResponse& exception)
    {
        ReportError("[Notificator] Cannot get number of pending notifications: %s",
                exception.what());

        return 0;
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Notificator] Cannot get number of pending notifications: %s",
                exception.what());

        throw exception;
    }

    return numberOfNotifications;
}

void
Dispatcher::Notificator::processPendingNotifications()
{
    APNS::Service& apns = APNS::Service::SharedInstance();

    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Notificator);

    try
    {
        PostgreSQL::Query query(*database.connection);

        query.execute(QueryAllPendingNotifications);

        if (query.numberOfRows() > 0)
        {
            apns.connect();

            query.assertNumberOfColumns(3);
            query.assertColumnOfType(0, PostgreSQL::INT8OID);
            query.assertColumnOfType(1, PostgreSQL::BYTEAOID);
            query.assertColumnOfType(2, PostgreSQL::JSONBOID);

            while (query.reachedLastRow() == false)
            {
                const unsigned long notificationId = query.popBIGINT();
                const char* deviceToken = query.popBYTEA();
                const std::string payload = query.popJSONB();

                ReportInfo("[Notificator] Fetched pending notification #%lu",
                        notificationId);

                try
                {
                    apns.send(deviceToken, payload.substr(1));
                }
                catch (APNS::CommunicationError&)
                {
                    PostgreSQL::Query update(*database.connection);

                    unsigned long notificationIdUpdate = htobe64(notificationId);

                    update.pushBIGINT(&notificationIdUpdate);
                    update.execute(QueryMarkNotificationAsRescheduled);

                    ReportInfo("[Notificator] Marked notification #%lu as 'rescheduled'",
                            notificationId);
                }

                {
                    PostgreSQL::Query update(*database.connection);

                    unsigned long notificationIdUpdate = htobe64(notificationId);

                    update.pushBIGINT(&notificationIdUpdate);
                    update.execute(QueryMarkNotificationAsSent);

                    ReportInfo("[Notificator] Marked notification #%lu as 'sent'",
                            notificationId);
                }

                query.nextRow();
            }

            apns.disconnect();
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::UnexpectedQueryResponse& exception)
    {
        ReportError("[Notificator] Cannot process pending notifications: %s",
                exception.what());
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Notificator] Cannot process pending notifications: %s",
                exception.what());

        throw exception;
    }
    catch (APNS::CommunicationError& exception)
    {
        ReportError("[Notificator] Cannot process pending notifications: %s",
                exception.what());
    }
}

/**
 * @brief   Thread handler for service.
 */
void
Dispatcher::Notificator::ThreadHandler(Dispatcher::Notificator* notificator)
{
    ReportNotice("[Notificator] Service thread has been started");

    for (;;)
    {
        ReportDebug("[Notificator] Awaiting trigger but maximal %u seconds",
                Dispatcher::RefreshNotificationsQueueInterval);

        notificator->waitForTriggerWithTimeout(
                std::chrono::seconds { Dispatcher::RefreshNotificationsQueueInterval } );

        notificator->processPendingNotifications();
    }

    ReportWarning("[Notificator] Service thread is going to quit");
}
