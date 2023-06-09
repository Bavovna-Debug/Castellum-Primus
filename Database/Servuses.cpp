// System definition files.
//
#include <endian.h>

// Common definition files.
//
#include "PostgreSQL/PostgreSQL.hpp"
#include "Toolkit/Report.h"

// Local definition files.
//
#include "Primus/Database/Database.hpp"
#include "Primus/Database/Servus.hpp"
#include "Primus/Database/Servuses.hpp"
#include "Primus/Database/Queries/Servus.h"

unsigned long
Database::Servuses::TotalNumber()
{
    unsigned long numberOfServuses;

    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        std::unique_lock<std::mutex> queueLock { database.lock };

        PostgreSQL::Query query(database.connection());

        query.execute(QueryTotalNumberOfServuses);

        query.assertNumberOfRows(1);
        query.assertNumberOfColumns(1);
        query.assertColumnOfType(0, PostgreSQL::INT8OID);

        numberOfServuses = query.popBIGINT();
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot get number of servuses: %s",
                exception.what());

        throw exception;
    }

    return numberOfServuses;
}

Database::Servus&
Database::Servuses::ServusByAuthenticator(const std::string& authenticator)
{
    unsigned long servusId;

    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        std::unique_lock<std::mutex> queueLock { database.lock };

        PostgreSQL::Query query(database.connection());

        query.pushUUID(&authenticator);
        query.execute(QuerySearchForServusByAuthenticator);

        query.assertNumberOfRows(1);
        query.assertNumberOfColumns(1);
        query.assertColumnOfType(0, PostgreSQL::INT8OID);

        servusId = query.popBIGINT();
    }
    catch (PostgreSQL::UnexpectedQueryResponse&)
    {
        throw Database::ServusNotFound();
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot find servus: %s",
                exception.what());

        throw exception;
    }

    return Database::Servuses::ServusById(servusId);
}

Database::Servus&
Database::Servuses::ServusByIndex(const unsigned long servusIndex)
{
    unsigned long servusId;

    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        std::unique_lock<std::mutex> queueLock { database.lock };

        PostgreSQL::Query query(database.connection());

        unsigned long servusIndexQuery = htobe64(servusIndex);

        query.pushBIGINT(&servusIndexQuery);
        query.execute(QuerySearchForServusByIndex);

        query.assertNumberOfRows(1);
        query.assertNumberOfColumns(1);
        query.assertColumnOfType(0, PostgreSQL::INT8OID);

        servusId = query.popBIGINT();
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot find servus: %s",
                exception.what());

        throw exception;
    }

    return Database::Servuses::ServusById(servusId);
}

Database::Servus&
Database::Servuses::ServusById(const unsigned long servusId)
{
    Database::Servus* servus = new Database::Servus(servusId);

    return *servus;
}

unsigned long
Database::Servuses::DefineServus(const std::string& title)
{
    unsigned long servusId;

    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        std::unique_lock<std::mutex> queueLock { database.lock };

        PostgreSQL::Transaction transaction(database.connection());

        // Create a new record for new activator.
        //
        {
            PostgreSQL::Query query(database.connection());

            query.pushVARCHAR(&title);
            query.execute(QueryInsertServus);

            query.assertNumberOfRows(1);
            query.assertNumberOfColumns(1);
            query.assertColumnOfType(0, PostgreSQL::INT8OID);

            servusId = query.popBIGINT();
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot define servus: %s",
                exception.what());

        throw exception;
    }

    return servusId;
}

void
Database::Servuses::ResetAllServuses()
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        std::unique_lock<std::mutex> queueLock { database.lock };

        PostgreSQL::Transaction transaction(database.connection());

        {
            PostgreSQL::Query query(database.connection());

            query.execute(QueryResetAllServuses);
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot reset online status for all servuses: %s",
                exception.what());

        throw exception;
    }
}

const std::string
Database::Servuses::ServusesAsXML()
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        std::unique_lock<std::mutex> queueLock { database.lock };

        PostgreSQL::Query query(database.connection());

        query.execute(QueryServusesAsXML);

        query.assertNumberOfRows(1);
        query.assertNumberOfColumns(1);
        query.assertColumnOfType(0, PostgreSQL::XMLOID);

        return query.popXML();
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot fetch list of servuses: %s",
                exception.what());

        throw exception;
    }
}
