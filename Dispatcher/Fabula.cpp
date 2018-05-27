// System definition files.
//
#include <string>

// Common definition files.
//
#include "PostgreSQL/PostgreSQL.hpp"
#include "Toolkit/Report.h"
#include "Toolkit/Times.hpp"

// Local definition files.
//
#include "Primus/Database/Database.hpp"
#include "Primus/Database/Queries/Fabula.h"
#include "Primus/Dispatcher/Fabula.hpp"

Dispatcher::Fabula::Fabula(const unsigned long fabulaId)
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Fabulatorium);

    try
    {
        PostgreSQL::Query query(*database.connection);

        unsigned long fabulaIdQuery = htobe64(fabulaId);

        query.pushBIGINT(&fabulaIdQuery);
        query.execute(QuerySearchForFabulaById);

        query.assertNumberOfRows(1);
        query.assertNumberOfColumns(7);
        query.assertColumnOfType(0, PostgreSQL::INT8OID);
        query.assertColumnOfType(1, PostgreSQL::UUIDOID);
        query.assertColumnOfType(2, PostgreSQL::TIMESTAMPOID);
        query.assertColumnOfType(3, PostgreSQL::INT8OID);
        query.assertColumnOfType(4, PostgreSQL::VARCHAROID);
        query.assertColumnOfType(5, PostgreSQL::INT2OID);
        query.assertColumnOfType(6, PostgreSQL::VARCHAROID);

        this->fabulaId          = query.popBIGINT();
        this->fabulaToken       = query.popUUID();
        this->originTimestamp   = query.popTIMESTAMP();
        this->servusId          = query.popBIGINT();
        this->originatorLabel   = query.popVARCHAR();
        this->severityLevel     = query.popSMALL();
        this->message           = query.popVARCHAR();
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Fabula] Cannot load fabula: %s",
                exception.what());

        throw exception;
    }
}

Dispatcher::Fabula::~Fabula()
{
    if (this->originTimestamp != nullptr)
        delete this->originTimestamp;
}
std::string
Dispatcher::Fabula::Enqueue(
    Toolkit::Timestamp&     originTimestamp,
    const unsigned long     servusId,
    const std::string&      originatorLabel,
    const unsigned short    severityLevel,
    const std::string&      message)
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Fabulatorium);

    try
    {
        PostgreSQL::Transaction transaction(*database.connection);

        {
            PostgreSQL::Query query(*database.connection);

            unsigned long servusIdQuery = htobe64(servusId);
            unsigned short severityLevelQuery = htobe16(severityLevel);

            query.pushTIMESTAMP(originTimestamp);
            query.pushBIGINT(&servusIdQuery);
            query.pushVARCHAR(&originatorLabel);
            query.pushSMALL(&severityLevelQuery);
            query.pushVARCHAR(&message);
            query.execute(QueryCreateFabula);

            query.assertNumberOfRows(1);
            query.assertNumberOfColumns(1);
            query.assertColumnOfType(0, PostgreSQL::UUIDOID);

            const std::string fabulaToken = query.popUUID();

            return fabulaToken;
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Fabula] Cannot update fabula: %s",
                exception.what());

        throw exception;
    }
}
