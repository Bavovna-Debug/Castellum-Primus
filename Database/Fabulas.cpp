// System definition files.
//
#include <endian.h>
#include <cstring>
#include <string>

// Common definition files.
//
#include "PostgreSQL/PostgreSQL.hpp"
#include "Toolkit/Report.h"

// Local definition files.
//
#include "Primus/Database/Database.hpp"
#include "Primus/Database/Fabula.hpp"
#include "Primus/Database/Fabulas.hpp"
#include "Primus/Database/Queries/Fabula.h"

unsigned long
Database::Fabulas::totalNumber()
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
        ReportError("[Database] Cannot get number of fabulas: %s",
                exception.what());

        throw exception;
    }

    return numberOfFabulas;
}

Database::Fabula&
Database::Fabulas::fabulaByIndex(const unsigned long fabulaIndex)
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
        ReportError("[Database] Cannot load fabula: %s",
                exception.what());

        throw exception;
    }

    return this->fabulaById(fabulaId);
}

Database::Fabula&
Database::Fabulas::fabulaById(const unsigned long fabulaId)
{
    Database::Fabula* fabula = new Database::Fabula(fabulaId);

    return *fabula;
}

const std::string
Database::Fabulas::FabulasAsXML(
    const std::string&  lastKnownFabulaToken,
    const unsigned int  maximalFabulasPerXML)
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Query query(*database.connection);

        unsigned int limitQuery = htobe32(maximalFabulasPerXML);

        if (lastKnownFabulaToken.empty() == true)
        {
            // Set second argument tu null in case fabula token has not been specified.
            //
            query.pushUUID();
        }
        else
        {
            query.pushUUID(&lastKnownFabulaToken);
        }
        query.pushINTEGER(&limitQuery);
        query.execute(QueryFabulasAsXML);

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
        ReportError("[Database] Cannot fetch list of fabulas: %s",
                exception.what());

        throw exception;
    }
}
