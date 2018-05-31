// System definition files.
//
#include <libconfig.h++>
#include <stdexcept>

// Common definition files.
//
#include "Toolkit/Report.h"

// Local definition files.
//
#include "Primus/Configuration.hpp"

static Primus::Configuration* instance = NULL;

Primus::Configuration&
Primus::Configuration::InitInstance(const std::string& configurationFilePath)
{
    if (instance != NULL)
        throw std::runtime_error("Configuration already initialized");

    instance = new Primus::Configuration(configurationFilePath);

    return *instance;
}

Primus::Configuration&
Primus::Configuration::SharedInstance()
{
    if (instance == NULL)
        throw std::runtime_error("Configuration not initialized");

    return *instance;
}

Primus::Configuration::Configuration(const std::string& configurationFilePath) :
configurationFilePath(configurationFilePath)
{
    this->http.portNumber                           = Primus::DefaultHTTPPortNumberIPv4;
    this->http.keepAliveSession                     = Primus::DefaultHTTPKeepAliveSession;

    this->servus.portNumberIPv4                     = Primus::DefaultServusPortNumberIPv4;
    this->servus.portNumberIPv6                     = Primus::DefaultServusPortNumberIPv6;
    this->servus.waitForFirstDatagram               = Primus::DefaultServusWaitForFirstDatagram;
    this->servus.waitForDatagramCompletion          = Primus::DefaultServusWaitForDatagramCompletion;
    this->servus.intervalBetweenNeutrinos           = Primus::DefaultServusIntervalBetweenNeutrinos;
    this->servus.finalWaitForNeutrino               = Primus::DefaultServusFinalWaitForNeutrino;

    this->phoenix.portNumberIPv4                    = Primus::DefaultPhoenixPortNumberIPv4;
    this->phoenix.portNumberIPv6                    = Primus::DefaultPhoenixPortNumberIPv6;
    this->phoenix.waitForFirstDatagram              = Primus::DefaultPhoenixWaitForFirstDatagram;
    this->phoenix.waitForDatagramCompletion         = Primus::DefaultPhoenixWaitForDatagramCompletion;
    this->phoenix.delayResponseForActivate          = Primus::DefaultPhoenixDelayResponseForActivate;
    this->phoenix.delayResponseForLogin             = Primus::DefaultPhoenixDelayResponseForLogin;
    this->phoenix.delayResponseForRejected          = Primus::DefaultPhoenixDelayResponseForRejected;
    this->phoenix.keepAlive                         = Primus::DefaultPhoenixKeepAlive;

    this->apns.delayAfterWakeup                     = Primus::DefaultDelayAfterWakeup;
    this->apns.delayBetweenFrames                   = Primus::DefaultDelayBetweenFrames;
    this->apns.delayAfterCompletion                 = Primus::DefaultDelayAfterCompletion;
    this->apns.pauseBeforeReconnect                 = Primus::DefaultPauseBeforeReconnect;

    this->load();
}

libconfig::Config&
Primus::Configuration::open()
{
    libconfig::Config* config = new libconfig::Config;
    config->readFile(this->configurationFilePath.c_str());
    config->setTabWidth(4);
    return *config;
}

void
Primus::Configuration::close(libconfig::Config& config)
{
    config.~Config();
}

void
Primus::Configuration::flush(libconfig::Config& config)
{
    config.writeFile(this->configurationFilePath.c_str());
}
