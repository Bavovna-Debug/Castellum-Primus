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
#include "Primus/Database/Servus.hpp"
#include "Primus/Database/Queries/Servus.h"

Database::Servus::Servus(const unsigned long servusId)
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Query query(*database.connection);

        unsigned long servusIdQuery = htobe64(servusId);

        query.pushBIGINT(&servusIdQuery);
        query.execute(QuerySearchForServusById);

        query.assertNumberOfRows(1);
        query.assertNumberOfColumns(7);
        query.assertColumnOfType(0, PostgreSQL::TIMESTAMPOID);
        query.assertColumnOfType(1, PostgreSQL::INT8OID);
        query.assertColumnOfType(2, PostgreSQL::UUIDOID);
        query.assertColumnOfType(3, PostgreSQL::BOOLOID);
        query.assertColumnOfType(4, PostgreSQL::BOOLOID);
        query.assertColumnOfType(5, PostgreSQL::UUIDOID);
        query.assertColumnOfType(6, PostgreSQL::VARCHAROID);

        this->timestamp     = query.popTIMESTAMP();
        this->servusId      = query.popBIGINT();
        this->token         = query.popUUID();
        this->enabled       = query.popBOOLEAN();
        this->online        = query.popBOOLEAN();
        this->authenticator = query.popUUID();
        this->description   = query.popVARCHAR();
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Servus] Cannot load servus: %s",
                exception.what());

        throw exception;
    }
}

Database::Servus::~Servus()
{
    if (this->timestamp != NULL)
    {
        delete this->timestamp;
    }
}

void
Database::Servus::setOnline()
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Transaction transaction(*database.connection);

        {
            PostgreSQL::Query query(*database.connection);

            unsigned long servusIdQuery = htobe64(this->servusId);

            query.pushBIGINT(&servusIdQuery);
            query.execute(QueryServusSetOnline);
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Servus] Cannot update servus online status: %s",
                exception.what());

        throw exception;
    }
}

void
Database::Servus::setOffline()
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Transaction transaction(*database.connection);

        {
            PostgreSQL::Query query(*database.connection);

            unsigned long servusIdQuery = htobe64(this->servusId);

            query.pushBIGINT(&servusIdQuery);
            query.execute(QueryServusSetOffline);
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Servus] Cannot update servus online status: %s",
                exception.what());

        throw exception;
    }
}

void
Database::Servus::toggleEnabledFlag()
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Transaction transaction(*database.connection);

        {
            PostgreSQL::Query query(*database.connection);

            unsigned long servusIdQuery = htobe64(this->servusId);

            query.pushBIGINT(&servusIdQuery);
            query.execute(QueryToggleServusEnabledFlag);

            query.assertNumberOfRows(1);
            query.assertNumberOfColumns(1);
            query.assertColumnOfType(0, PostgreSQL::BOOLOID);

            this->enabled = query.popBOOLEAN();
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Servus] Cannot toggle servus enabled flag: %s",
                exception.what());

        throw exception;
    }
}

void
Database::Servus::setDescription(const std::string& description)
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Transaction transaction(*database.connection);

        {
            PostgreSQL::Query query(*database.connection);

            unsigned long servusIdQuery = htobe64(this->servusId);

            query.pushBIGINT(&servusIdQuery);
            query.pushVARCHAR(&description);
            query.execute(QueryUpdateServusDescription);

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
        ReportError("[Servus] Cannot update servus: %s",
                exception.what());

        throw exception;
    }
}

unsigned long
Database::Servus::DefineServus(const std::string& description)
{
    unsigned long servusId;

    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Transaction transaction(*database.connection);

        // Create a new record for new activator.
        //
        {
            PostgreSQL::Query query(*database.connection);

            query.pushVARCHAR(&description);
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
        ReportError("[Activator] Cannot define servus: %s",
                exception.what());

        throw exception;
    }

    return servusId;
}
