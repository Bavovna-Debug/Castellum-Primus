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
#include "Primus/Database/Activator.hpp"
#include "Primus/Database/Activators.hpp"
#include "Primus/Database/Queries/Activator.h"

unsigned long
Database::Activators::TotalNumber()
{
    unsigned long numberOfActivators;

    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        std::unique_lock<std::mutex> queueLock { database.lock };

        PostgreSQL::Query query(database.connection());

        query.execute(QueryTotalNumberOfActivators);

        query.assertNumberOfRows(1);
        query.assertNumberOfColumns(1);
        query.assertColumnOfType(0, PostgreSQL::INT8OID);

        numberOfActivators = query.popBIGINT();
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot get number of activators: %s",
                exception.what());

        throw exception;
    }

    return numberOfActivators;
}

Database::Activator&
Database::Activators::ActivatorByIndex(const unsigned long activatorIndex)
{
    unsigned long activatorId;

    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        std::unique_lock<std::mutex> queueLock { database.lock };

        PostgreSQL::Query query(database.connection());

        unsigned long activatorIndexQuery = htobe64(activatorIndex);

        query.pushBIGINT(&activatorIndexQuery);
        query.execute(QuerySearchForActivatorByIndex);

        query.assertNumberOfRows(1);
        query.assertNumberOfColumns(1);
        query.assertColumnOfType(0, PostgreSQL::INT8OID);

        activatorId = query.popBIGINT();
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot find activator: %s",
                exception.what());

        throw exception;
    }

    return Database::Activators::ActivatorById(activatorId);
}

Database::Activator&
Database::Activators::ActivatorById(const unsigned long activatorId)
{
    Database::Activator* activator = new Database::Activator(activatorId);

    return *activator;
}
