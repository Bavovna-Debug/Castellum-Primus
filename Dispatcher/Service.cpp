// System definition files.
//
#include <stdexcept>

// Common definition files.
//
#include "Communicator/DNS.hpp"

// Local definition files.
//
#include "Primus/Configuration.hpp"
#include "Primus/Database/Servuses.hpp"
#include "Primus/Dispatcher/Listener.hpp"
#include "Primus/Dispatcher/Service.hpp"

static Dispatcher::Service* instance = NULL;

Dispatcher::Service&
Dispatcher::Service::InitInstance()
{
    if (instance != NULL)
        throw std::runtime_error("[Dispatcher] Service already initialized");

    instance = new Dispatcher::Service();

    return *instance;
}

Dispatcher::Service&
Dispatcher::Service::SharedInstance()
{
    if (instance == NULL)
        throw std::runtime_error("[Dispatcher] Service not initialized");

    return *instance;
}

Dispatcher::Service::Service()
{
    // Before starting dispatcher reset online status of all Servuses in database.
    // Online status could be left "online" since last runtime of Primus.
    //
    Database::Servuses::ResetAllServuses();

    Primus::Configuration& configuration = Primus::Configuration::SharedInstance();

    this->listenerIPv4 = new Dispatcher::Listener(
            IP::IPv4,
            configuration.servus.portNumberIPv4);

    this->listenerIPv6 = new Dispatcher::Listener(
            IP::IPv6,
            configuration.servus.portNumberIPv6);
}
