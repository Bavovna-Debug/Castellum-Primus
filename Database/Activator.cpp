// System definition files.
//
#include <string>

// Common definition files.
//
#include "APNS/APNS.hpp"
#include "PostgreSQL/PostgreSQL.hpp"
#include "Toolkit/Report.h"
#include "Toolkit/Times.hpp"

// Local definition files.
//
#include "Primus/Database/Database.hpp"
#include "Primus/Database/Activator.hpp"
#include "Primus/Database/Queries/Activator.h"

Database::Activator::Activator(const unsigned long activatorId)
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Query query(*database.connection);

        unsigned long activatorIdQuery = htobe64(activatorId);

        query.pushBIGINT(&activatorIdQuery);
        query.execute(QuerySearchForActivatorById);

        query.assertNumberOfRows(1);
        query.assertNumberOfColumns(6);
        query.assertColumnOfType(0, PostgreSQL::TIMESTAMPOID);
        query.assertColumnOfType(1, PostgreSQL::INT8OID);
        query.assertColumnOfType(2, PostgreSQL::UUIDOID);
        query.assertColumnOfType(3, PostgreSQL::INT8OID);
        query.assertColumnOfType(4, PostgreSQL::VARCHAROID);
        query.assertColumnOfType(5, PostgreSQL::VARCHAROID);

        this->timestamp         = query.popTIMESTAMP();
        this->activatorId       = query.popBIGINT();
        this->activatorToken    = query.popUUID();

        if (query.cellIsNull() == true)
        {
            this->phoenixId = 0;
            query.nextColumn();
        }
        else
        {
            this->phoenixId = query.popBIGINT();
        }

        this->activationCode = query.popVARCHAR();
        this->title = query.popVARCHAR();
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot load activator: %s",
                exception.what());

        throw exception;
    }
}

Database::Activator::~Activator()
{
    if (this->timestamp != NULL)
    {
        delete this->timestamp;
    }
}

void
Database::Activator::setActivationCode(const std::string& activationCode)
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Transaction transaction(*database.connection);

        {
            PostgreSQL::Query query(*database.connection);

            unsigned long activatorIdQuery = htobe64(this->activatorId);

            query.pushBIGINT(&activatorIdQuery);
            query.pushVARCHAR(&activationCode);
            query.execute(QueryUpdateActivatorActivationCode);
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot change activation code: %s",
                exception.what());

        throw exception;
    }
}

void
Database::Activator::setTitle(const std::string& title)
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Transaction transaction(*database.connection);

        {
            PostgreSQL::Query query(*database.connection);

            unsigned long activatorIdQuery = htobe64(this->activatorId);

            query.pushBIGINT(&activatorIdQuery);
            query.pushVARCHAR(&title);
            query.execute(QueryUpdateActivatorTitle);
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot update activator: %s",
                exception.what());

        throw exception;
    }
}

unsigned long
Database::Activator::DefineActivator(
    const std::string&  activationCode,
    const std::string&  title)
{
    unsigned long activatorId;

    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Transaction transaction(*database.connection);

        // Create a new record for new activator.
        //
        {
            PostgreSQL::Query query(*database.connection);

            query.pushVARCHAR(&activationCode);
            query.pushVARCHAR(&title);
            query.execute(QueryInsertActivator);

            query.assertNumberOfRows(1);
            query.assertNumberOfColumns(1);
            query.assertColumnOfType(0, PostgreSQL::INT8OID);

            activatorId = query.popBIGINT();
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot define activator: %s",
                exception.what());

        throw exception;
    }

    return activatorId;
}
