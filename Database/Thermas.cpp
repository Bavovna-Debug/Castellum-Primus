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
#include "Primus/Database/Therma.hpp"
#include "Primus/Database/Thermas.hpp"
#include "Primus/Database/Queries/Therma.h"

unsigned long
Database::Thermas::TotalNumber()
{
    unsigned long numberOfThermas;

    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Query query(*database.connection);

        query.execute(QueryTotalNumberOfThermas);

        query.assertNumberOfRows(1);
        query.assertNumberOfColumns(1);
        query.assertColumnOfType(0, PostgreSQL::INT8OID);

        numberOfThermas = query.popBIGINT();
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Thermas] Cannot get number of thermas: %s",
                exception.what());

        throw exception;
    }

    return numberOfThermas;
}

Database::Therma&
Database::Thermas::ThermaByIndex(const unsigned long thermaIndex)
{
    unsigned long thermaId;

    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Query query(*database.connection);

        unsigned long thermaIndexQuery = htobe64(thermaIndex);

        query.pushBIGINT(&thermaIndexQuery);
        query.execute(QuerySearchForThermaByIndex);

        query.assertNumberOfRows(1);
        query.assertNumberOfColumns(1);
        query.assertColumnOfType(0, PostgreSQL::INT8OID);

        thermaId = query.popBIGINT();
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Thermas] Cannot load therma: %s",
                exception.what());

        throw exception;
    }

    return Database::Thermas::ThermaById(thermaId);
}

Database::Therma&
Database::Thermas::ThermaById(const unsigned long thermaId)
{
    Database::Therma* therma = new Database::Therma(thermaId);

    return *therma;
}
