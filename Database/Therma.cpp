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
#include "Primus/Database/Therma.hpp"
#include "Primus/Database/Queries/Therma.h"

Database::Therma::Therma(const unsigned long thermaId)
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Query query(*database.connection);

        unsigned long thermaIdQuery = htobe64(thermaId);

        query.pushBIGINT(&thermaIdQuery);
        query.execute(QuerySearchForThermaById);

        query.assertNumberOfRows(1);
        query.assertNumberOfColumns(6);
        query.assertColumnOfType(0, PostgreSQL::TIMESTAMPOID);
        query.assertColumnOfType(1, PostgreSQL::INT8OID);
        query.assertColumnOfType(2, PostgreSQL::UUIDOID);
        query.assertColumnOfType(3, PostgreSQL::INT8OID);
        query.assertColumnOfType(4, PostgreSQL::BPCHAROID);
        query.assertColumnOfType(5, PostgreSQL::VARCHAROID);

        this->timestamp         = query.popTIMESTAMP();
        this->thermaId          = query.popBIGINT();
        this->token             = query.popUUID();
        this->servusId          = query.popBIGINT();
        this->gpioDeviceNumber  = query.popCHAR();
        this->description       = query.popVARCHAR();
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot load therma: %s",
                exception.what());

        throw exception;
    }
}

Database::Therma::~Therma()
{
    if (this->timestamp != NULL)
    {
        delete this->timestamp;
    }
}

void
Database::Therma::setDescription(const std::string& description)
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Transaction transaction(*database.connection);

        {
            PostgreSQL::Query query(*database.connection);

            unsigned long thermaIdQuery = htobe64(this->thermaId);

            query.pushBIGINT(&thermaIdQuery);
            query.pushVARCHAR(&description);
            query.execute(QueryUpdateThermaDescription);

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
        ReportError("[Database] Cannot update therma: %s",
                exception.what());

        throw exception;
    }
}
