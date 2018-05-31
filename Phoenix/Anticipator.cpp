// Common definition files.
//
#include "Toolkit/Report.h"

// Local definition files.
//
#include "Primus/Configuration.hpp"
#include "Primus/Phoenix/Anticipator.hpp"
#include "Primus/Phoenix/Listener.hpp"

static Phoenix::Anticipator* instance = NULL;

Phoenix::Anticipator&
Phoenix::Anticipator::InitInstance()
{
    if (instance != NULL)
        throw std::runtime_error("Anticipator already initialized");

    instance = new Phoenix::Anticipator();

    return *instance;
}

Phoenix::Anticipator&
Phoenix::Anticipator::SharedInstance()
{
    if (instance == NULL)
        throw std::runtime_error("Anticipator not initialized");

    return *instance;
}

Phoenix::Anticipator::Anticipator()
{
    Primus::Configuration& configuration = Primus::Configuration::SharedInstance();

    this->listenerIPv4 = new Phoenix::Listener(
            IP::IPv4,
            configuration.phoenix.portNumberIPv4);

    this->listenerIPv6 = new Phoenix::Listener(
            IP::IPv6,
            configuration.phoenix.portNumberIPv6);
}
