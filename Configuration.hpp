#pragma once

// System definition files.
//
#include <libconfig.h++>
#include <cstdbool>
#include <string>

namespace Primus
{
    static const std::string InstanceName       = "primus";

    static const std::string SoftwareVersion    = "Primus 0.4 [180611]";

    static const unsigned DefaultHTTPPortNumberIPv4                 = 15080;
    static const unsigned DefaultHTTPKeepAliveSession               = 300;      /**< Seconds. */

    static const unsigned DefaultServusPortNumberIPv4               = 15024;
    static const unsigned DefaultServusPortNumberIPv6               = 15026;
    static const unsigned DefaultPhoenixPortNumberIPv4              = 15004;
    static const unsigned DefaultPhoenixPortNumberIPv6              = 15006;

    static const unsigned DefaultServusWaitForFirstDatagram         = 5000;     /**< Milliseconds. */
    static const unsigned DefaultServusWaitForDatagramCompletion    = 2000;     /**< Milliseconds. */
    static const unsigned DefaultServusIntervalBetweenNeutrinos     = 30000;    /**< Milliseconds. */
    static const unsigned DefaultServusFinalWaitForNeutrino         = 2000;     /**< Milliseconds. */

    static const unsigned DefaultPhoenixWaitForFirstDatagram        = 5000;     /**< Milliseconds. */
    static const unsigned DefaultPhoenixWaitForDatagramCompletion   = 2000;     /**< Milliseconds. */
    static const unsigned DefaultPhoenixDelayResponseForActivate    = 2000;     /**< Milliseconds. */
    static const unsigned DefaultPhoenixDelayResponseForLogin       = 200;      /**< Milliseconds. */
    static const unsigned DefaultPhoenixDelayResponseForRejected    = 1000;      /**< Milliseconds. */
    static const unsigned DefaultPhoenixKeepAlive                   = 300000;   /**< Milliseconds. */

    static const unsigned DefaultDelayAfterWakeup                   = 500;      /**< Milliseconds. */
    static const unsigned DefaultDelayBetweenFrames                 = 100;      /**< Milliseconds. */
    static const unsigned DefaultDelayAfterCompletion               = 500;      /**< Milliseconds. */
    static const unsigned DefaultPauseBeforeReconnect               = 10;

    static const unsigned int ActivationCodeLength                  = 10;

    static const unsigned int MaximalNumberOfFabulasPerXML          = 20;

    class Configuration
    {
    public:
        std::string             configurationFilePath;

        struct
        {
            std::string         hostName;
            unsigned short      portNumber;
            std::string         databaseName;
            std::string         role;
            std::string         password;
        }
        database;

        struct
        {
            unsigned short      portNumber;
            std::string         passwordMD5;
            unsigned int        keepAliveSession;
        }
        http;

        struct
        {
            unsigned short      portNumberIPv4;
            unsigned short      portNumberIPv6;
            unsigned int        waitForFirstDatagram;
            unsigned int        waitForDatagramCompletion;
            unsigned int        intervalBetweenNeutrinos;
            unsigned int        finalWaitForNeutrino;
        }
        servus;

        struct
        {
            std::string         interfaceAddress;
            unsigned short      portNumberIPv4;
            unsigned short      portNumberIPv6;
            unsigned int        waitForFirstDatagram;
            unsigned int        waitForDatagramCompletion;
            unsigned int        delayResponseForActivate;
            unsigned int        delayResponseForLogin;
            unsigned int        delayResponseForRejected;
            unsigned int        keepAlive;
        }
        phoenix;

        struct
        {
            bool                sandbox;
            std::string         certificate;
            unsigned int        delayAfterWakeup;
            unsigned int        delayBetweenFrames;
            unsigned int        delayAfterCompletion;
            unsigned int        pauseBeforeReconnect;
        }
        apns;

    public:
        static Primus::Configuration&
        InitInstance(const std::string& configurationFilePath);

        static Primus::Configuration&
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
