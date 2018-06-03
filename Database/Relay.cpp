// System definition files.
//
#include <cstdbool>
#include <string>

// Common definition files.
//
#include "PostgreSQL/PostgreSQL.hpp"
#include "Toolkit/Report.h"
#include "Toolkit/Times.hpp"

// Local definition files.
//
#include "Primus/Database/Database.hpp"
#include "Primus/Database/Relay.hpp"
#include "Primus/Database/Queries/Relay.h"

Database::Relay::Relay(const unsigned long relayId)
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Query query(*database.connection);

        unsigned long relayIdQuery = htobe64(relayId);

        query.pushBIGINT(&relayIdQuery);
        query.execute(QuerySearchForRelayById);

        query.assertNumberOfRows(1);
        query.assertNumberOfColumns(7);
        query.assertColumnOfType(0, PostgreSQL::TIMESTAMPOID);
        query.assertColumnOfType(1, PostgreSQL::INT8OID);
        query.assertColumnOfType(2, PostgreSQL::UUIDOID);
        query.assertColumnOfType(3, PostgreSQL::INT8OID);
        query.assertColumnOfType(4, PostgreSQL::INT4OID);
        query.assertColumnOfType(5, PostgreSQL::BOOLOID);
        query.assertColumnOfType(6, PostgreSQL::VARCHAROID);

        this->timestamp         = query.popTIMESTAMP();
        this->relayId           = query.popBIGINT();
        this->token             = query.popUUID();
        this->servusId          = query.popBIGINT();
        this->gpioPinNumber     = query.popINTEGER();
        this->state             = query.popBOOLEAN();
        this->description       = query.popVARCHAR();
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot load relay: %s",
                exception.what());

        throw exception;
    }
}

Database::Relay::~Relay()
{
    if (this->timestamp != NULL)
    {
        delete this->timestamp;
    }
}

void
Database::Relay::setDescription(const std::string& description)
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Transaction transaction(*database.connection);

        {
            PostgreSQL::Query query(*database.connection);

            unsigned long relayIdQuery = htobe64(this->relayId);

            query.pushBIGINT(&relayIdQuery);
            query.pushVARCHAR(&description);
            query.execute(QueryUpdateRelayDescription);

            query.assertNumberOfRows(1);
            query.assertNumberOfColumns(1);
            query.assertColumnOfType(0, PostgreSQL::VARCHAROID);

            this->description = query.popVARCHAR();
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot update relay: %s",
                exception.what());

        throw exception;
    }
}

bool
Database::Relay::isOff()
{
    return (this->state == false);
}

bool
Database::Relay::isOn()
{
    return (this->state == true);
}

void
Database::Relay::switchOff()
{ }

void
Database::Relay::switchOn()
{ }

void
Database::Relay::switchOver()
{ }
