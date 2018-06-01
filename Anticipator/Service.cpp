// System definition files.
//
#include <endian.h>
#include <arpa/inet.h>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <stdexcept>

// Common definition files.
//
#include "Communicator/DNS.hpp"

// Local definition files.
//
#include "Primus/Configuration.hpp"
#include "Primus/Anticipator/Listener.hpp"
#include "Primus/Anticipator/Service.hpp"

static Anticipator::Service* instance = NULL;

Anticipator::Service&
Anticipator::Service::InitInstance()
{
    if (instance != NULL)
        throw std::runtime_error("[Anticipator] Service already initialized");

    instance = new Anticipator::Service();

    return *instance;
}

Anticipator::Service&
Anticipator::Service::SharedInstance()
{
    if (instance == NULL)
        throw std::runtime_error("[Anticipator] Service not initialized");

    return *instance;
}

Anticipator::Service::Service()
{
    Primus::Configuration& configuration = Primus::Configuration::SharedInstance();

    this->listenerIPv4 = new Anticipator::Listener(
            IP::IPv4,
            configuration.phoenix.portNumberIPv4);

    this->listenerIPv6 = new Anticipator::Listener(
            IP::IPv6,
            configuration.phoenix.portNumberIPv6);
}

const std::string
Anticipator::ServerKey()
{
    Primus::Configuration& configuration = Primus::Configuration::SharedInstance();

    in_addr_t binaryAddress;

    DNS::HostNameToIP(configuration.phoenix.interfaceAddress, 0, &binaryAddress);

    uint64_t source;
    source = be32toh(binaryAddress);
    source <<= 16;
    source |= configuration.phoenix.portNumberIPv4;

    std::stringstream stream;
    stream << std::oct << source;

    std::string serverKey = stream.str();

    for (size_t offset = 0;
         offset < serverKey.length();
         offset++)
    {
        size_t position;

        if ((position = serverKey.find("0", 0)) != std::string::npos)
        {
            serverKey.replace(position, 1, "A");
        }
        else if ((position = serverKey.find("1", 0)) != std::string::npos)
        {
            serverKey.replace(position, 1, "B");
        }
        else if ((position = serverKey.find("2", 0)) != std::string::npos)
        {
            serverKey.replace(position, 1, "C");
        }
        else if ((position = serverKey.find("3", 0)) != std::string::npos)
        {
            serverKey.replace(position, 1, "D");
        }
        else if ((position = serverKey.find("4", 0)) != std::string::npos)
        {
            serverKey.replace(position, 1, "E");
        }
        else if ((position = serverKey.find("5", 0)) != std::string::npos)
        {
            serverKey.replace(position, 1, "F");
        }
        else if ((position = serverKey.find("6", 0)) != std::string::npos)
        {
            serverKey.replace(position, 1, "G");
        }
        else if ((position = serverKey.find("7", 0)) != std::string::npos)
        {
            serverKey.replace(position, 1, "H");
        }
    }

    serverKey.insert(12, "-");
    serverKey.insert(8, "-");
    serverKey.insert(4, "-");

    return serverKey;
}
