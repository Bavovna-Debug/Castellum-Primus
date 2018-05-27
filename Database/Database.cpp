// System definition files.
//
#include <stdexcept>
#include <system_error>

// Common definition files.
//
#include "PostgreSQL/Exception.hpp"
#include "PostgreSQL/PostgreSQL.hpp"

// Local definition files.
//
#include "Primus/Configuration.hpp"
#include "Primus/Database/Database.hpp"

static Primus::Database* instance[Primus::Database::NumberOfInstances] = { NULL };

Primus::Database&
Primus::Database::InitInstance(const Primus::Database::Instance instanceId)
{
    if (instance[instanceId] != NULL)
        throw std::runtime_error("Database already initialized");

    instance[instanceId] = new Primus::Database();

    return *instance[instanceId];
}

Primus::Database&
Primus::Database::SharedInstance(const Primus::Database::Instance instanceId)
{
    if (instance[instanceId] == NULL)
        throw std::runtime_error("Database not initialized");

    return *instance[instanceId];
}

Primus::Database::Database()
{
    this->connect();
}

Primus::Database::~Database()
{ }

void
Primus::Database::connect()
{
    Primus::Configuration& configuration = Primus::Configuration::SharedInstance();

    this->connection = new PostgreSQL::Connection(
            configuration.database.hostName->c_str(),
            configuration.database.portNumber,
            configuration.database.databaseName->c_str(),
            configuration.database.role->c_str(),
            configuration.database.password->c_str());
}

void
Primus::Database::disconnect()
{
    delete this->connection;
}

void
Primus::Database::recover(PostgreSQL::OperatorIntervention& exception)
{
    if (!exception.queryCanceled())
    {
        this->disconnect();
        this->connect();
    }
}
