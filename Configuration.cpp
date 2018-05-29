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
    this->httpPortNumber             = Primus::DefaultHTTPPortNumber;
    this->servusPortNumberIPv4       = Primus::DefaultServusPortNumberIPv4;
    this->servusPortNumberIPv6       = Primus::DefaultServusPortNumberIPv6;
    this->anticipatorPortNumberIPv4  = Primus::DefaultAnticipatorPortNumberIPv4;
    this->anticipatorPortNumberIPv6  = Primus::DefaultAnticipatorPortNumberIPv6;

    this->network.servus.waitForFirstDatagram       = Primus::DefaultServusWaitForFirstDatagram;
    this->network.servus.waitForDatagramCompletion  = Primus::DefaultServusWaitForDatagramCompletion;
    this->network.servus.intervalBetweenNeutrinos   = Primus::DefaultServusIntervalBetweenNeutrinos;
    this->network.servus.finalWaitForNeutrino       = Primus::DefaultServusFinalWaitForNeutrino;

    this->network.phoenix.waitForFirstDatagram      = Primus::DefaultPhoenixWaitForFirstDatagram;
    this->network.phoenix.waitForDatagramCompletion = Primus::DefaultPhoenixWaitForDatagramCompletion;
    this->network.phoenix.delayResponseForActivate  = Primus::DefaultPhoenixDelayResponseForActivate;
    this->network.phoenix.delayResponseForLogin     = Primus::DefaultPhoenixDelayResponseForLogin;
    this->network.phoenix.delayResponseForRejected  = Primus::DefaultPhoenixDelayResponseForRejected;
    this->network.phoenix.keepAlive                 = Primus::DefaultPhoenixKeepAlive;

    this->apns.delayAfterWakeup      = Primus::DefaultDelayAfterWakeup;
    this->apns.delayBetweenFrames    = Primus::DefaultDelayBetweenFrames;
    this->apns.delayAfterCompletion  = Primus::DefaultDelayAfterCompletion;
    this->apns.pauseBeforeReconnect  = Primus::DefaultPauseBeforeReconnect;

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
