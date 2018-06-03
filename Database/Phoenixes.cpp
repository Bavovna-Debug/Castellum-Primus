// System definition files.
//
#include <endian.h>
#include <string>

// Common definition files.
//
#include "PostgreSQL/PostgreSQL.hpp"
#include "Toolkit/Report.h"

// Local definition files.
//
#include "Primus/Database/Database.hpp"
#include "Primus/Database/Phoenix.hpp"
#include "Primus/Database/Phoenixes.hpp"
#include "Primus/Database/Queries/Phoenix.h"

unsigned long
Database::Phoenixes::TotalNumber()
{
    unsigned long numberOfPhoenixes;

    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Query query(*database.connection);

        query.execute(QueryTotalNumberOfPhoenixes);

        query.assertNumberOfRows(1);
        query.assertNumberOfColumns(1);
        query.assertColumnOfType(0, PostgreSQL::INT8OID);

        numberOfPhoenixes = query.popBIGINT();
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot get number of phoenixes: %s",
                exception.what());

        throw exception;
    }

    return numberOfPhoenixes;
}

Database::Phoenix&
Database::Phoenixes::PhoenixByIndex(const unsigned long phoenixIndex)
{
    unsigned long phoenixId;

    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Query query(*database.connection);

        unsigned long phoenixIndexQuery = htobe64(phoenixIndex);

        query.pushBIGINT(&phoenixIndexQuery);
        query.execute(QuerySearchForPhoenixByIndex);

        query.assertNumberOfRows(1);
        query.assertNumberOfColumns(1);
        query.assertColumnOfType(0, PostgreSQL::INT8OID);

        phoenixId = query.popBIGINT();
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot find phoenix: %s",
                exception.what());

        throw exception;
    }

    return Database::Phoenixes::PhoenixById(phoenixId);
}

Database::Phoenix&
Database::Phoenixes::PhoenixByToken(const std::string& phoenixToken)
{
    unsigned long phoenixId;

    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Query query(*database.connection);

        query.pushUUID(&phoenixToken);
        query.execute(QuerySearchForPhoenixByToken);

        query.assertNumberOfRows(1);
        query.assertNumberOfColumns(1);
        query.assertColumnOfType(0, PostgreSQL::INT8OID);

        phoenixId = query.popBIGINT();
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot find phoenix: %s",
                exception.what());

        throw exception;
    }

    return Database::Phoenixes::PhoenixById(phoenixId);
}

Database::Phoenix&
Database::Phoenixes::PhoenixById(const unsigned long phoenixId)
{
    Database::Phoenix* phoenix = new Database::Phoenix(phoenixId);

    return *phoenix;
}

void
Database::Phoenixes::RemovePhoenixById(const unsigned long phoenixId)
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Transaction transaction(*database.connection);

        {
            PostgreSQL::Query query(*database.connection);

            unsigned long phoenixIdQuery = htobe64(phoenixId);

            query.pushBIGINT(&phoenixIdQuery);
            query.execute(QueryRemovePhoenixById);
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot remove phoenix: %s",
                exception.what());

        throw exception;
    }
}
