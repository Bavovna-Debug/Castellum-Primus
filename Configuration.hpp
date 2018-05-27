#pragma once

// System definition files.
//
#include <libconfig.h++>
#include <cstdbool>
#include <string>

namespace Primus
{
    static const std::string SoftwareVersion = "Primus 0.0 [180526]";

    static const unsigned DefaultHTTPPortNumber                     = 15080;
    static const unsigned DefaultServusPortNumberIPv4               = 15024;
    static const unsigned DefaultServusPortNumberIPv6               = 15026;
    static const unsigned DefaultAnticipatorPortNumberIPv4          = 15004;
    static const unsigned DefaultAnticipatorPortNumberIPv6          = 15006;

    static const unsigned DefaultServusWaitForFirstDatagram         = 5000;     /**< Milliseconds. */
    static const unsigned DefaultServusWaitForDatagramCompletion    = 2000;     /**< Milliseconds. */
    static const unsigned DefaultServusIntervalBetweenNeutrinos     = 30000;    /**< Milliseconds. */
    static const unsigned DefaultServusFinalWaitForNeutrino         = 2000;     /**< Milliseconds. */

    static const unsigned DefaultPhoenixWaitForFirstDatagram        = 5000;     /**< Milliseconds. */
    static const unsigned DefaultPhoenixWaitForDatagramCompletion   = 2000;     /**< Milliseconds. */
    static const unsigned DefaultPhoenixKeepAlive                   = 300000;   /**< Milliseconds. */

    static const unsigned DefaultDelayAfterWakeup                   = 500;      /**< Milliseconds. */
    static const unsigned DefaultDelayBetweenFrames                 = 100;      /**< Milliseconds. */
    static const unsigned DefaultDelayAfterCompletion               = 500;      /**< Milliseconds. */
    static const unsigned DefaultPauseBeforeReconnect               = 10;

    static const unsigned int ActivationCodeLength = 10;

    class Configuration
    {
    public:
        std::string             configurationFilePath;

        struct
        {
            std::string*        hostName;
            unsigned int        portNumber;
            std::string*        databaseName;
            std::string*        role;
            std::string*        password;
        }
        database;

        unsigned int            httpPortNumber;
        unsigned int            servusPortNumberIPv4;
        unsigned int            servusPortNumberIPv6;
        unsigned int            anticipatorPortNumberIPv4;
        unsigned int            anticipatorPortNumberIPv6;

        struct
        {
            struct
            {
                unsigned int    waitForFirstDatagram;
                unsigned int    waitForDatagramCompletion;
                unsigned int    intervalBetweenNeutrinos;
                unsigned int    finalWaitForNeutrino;
            }
            servus;

            struct
            {
                unsigned int    waitForFirstDatagram;
                unsigned int    waitForDatagramCompletion;
                unsigned int    keepAlive;
            }
            phoenix;
        }
        network;

        struct
        {
            bool                sandbox;
            std::string*        certificate;
            unsigned int        delayAfterWakeup;
            unsigned int        delayBetweenFrames;
            unsigned int        delayAfterCompletion;
            unsigned int        pauseBeforeReconnect;
        }
        apns;

    public:
        static Configuration&
        InitInstance(const std::string& configurationFilePath);

        static Configuration&
        SharedInstance();

    private:
        Configuration(const std::string& configurationFilePath);

        void
        load();

    private:
        libconfig::Config&
        open();

        void
        close(libconfig::Config&);

        void
        flush(libconfig::Config&);
    };
};
