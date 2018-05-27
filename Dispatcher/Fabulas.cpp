// System definition files.
//
#include <endian.h>
#include <cstring>

// Common definition files.
//
#include "PostgreSQL/PostgreSQL.hpp"
#include "Toolkit/Report.h"

// Local definition files.
//
#include "Primus/Database/Database.hpp"
#include "Primus/Database/Queries/Fabula.h"
#include "Primus/Dispatcher/Fabula.hpp"
#include "Primus/Dispatcher/Fabulas.hpp"

unsigned long
Dispatcher::Fabulas::totalNumber()
{
    unsigned long numberOfFabulas;

    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Fabulatorium);

    try
    {
        PostgreSQL::Query query(*database.connection);

        query.execute(QueryTotalNumberOfFabulas);

        query.assertNumberOfRows(1);
        query.assertNumberOfColumns(1);
        query.assertColumnOfType(0, PostgreSQL::INT8OID);

        numberOfFabulas = query.popBIGINT();
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Fabulas] Cannot get number of fabulas: %s",
                exception.what());

        throw exception;
    }

    return numberOfFabulas;
}

Dispatcher::Fabula&
Dispatcher::Fabulas::fabulaByIndex(const unsigned long fabulaIndex)
{
    unsigned long fabulaId;

    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Fabulatorium);

    try
    {
        PostgreSQL::Query query(*database.connection);

        unsigned long fabulaIndexQuery = htobe64(fabulaIndex);

        query.pushBIGINT(&fabulaIndexQuery);
        query.execute(QuerySearchForFabulaByIndex);

        query.assertNumberOfRows(1);
        query.assertNumberOfColumns(1);
        query.assertColumnOfType(0, PostgreSQL::INT8OID);

        fabulaId = query.popBIGINT();
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Fabulas] Cannot load fabula: %s",
                exception.what());

        throw exception;
    }

    return this->fabulaById(fabulaId);
}

Dispatcher::Fabula&
Dispatcher::Fabulas::fabulaById(const unsigned long fabulaId)
{
    Dispatcher::Fabula* fabula = new Dispatcher::Fabula(fabulaId);

    return *fabula;
}
