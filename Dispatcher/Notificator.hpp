#pragma once

// System definition files.
//
#include <condition_variable>
#include <mutex>
#include <thread>

namespace Dispatcher
{
    static const unsigned int RefreshNotificationsQueueInterval = 60; /**< Seconds. */

    class Notificator
    {
    private:
        /**
         * Thread handler of service thread.
         */
        std::thread thread;

        struct
        {
            std::mutex              lock;
            std::condition_variable condition;
        }
        trigger;

    public:
        static Dispatcher::Notificator&
        InitInstance();

        static Dispatcher::Notificator&
        SharedInstance();

        void
        startService();

        void
        triggerProcessing();

        std::cv_status
        waitForTriggerWithTimeout(const std::chrono::seconds duration);

        unsigned long
        numberOfPendingNotifications();

        void
        processPendingNotifications();

    private:
        static void
        ThreadHandler(Dispatcher::Notificator*);
    };
};
