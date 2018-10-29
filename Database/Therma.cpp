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
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Sensors);

    try
    {
        std::unique_lock<std::mutex> queueLock { database.lock };

        PostgreSQL::Query query(database.connection());

        unsigned long thermaIdQuery = htobe64(thermaId);

        query.pushBIGINT(&thermaIdQuery);
        query.execute(QuerySearchForDSSensorById);

        query.assertNumberOfRows(1);
        query.assertNumberOfColumns(7);
        query.assertColumnOfType(0, PostgreSQL::TIMESTAMPOID);
        query.assertColumnOfType(1, PostgreSQL::INT8OID);
        query.assertColumnOfType(2, PostgreSQL::UUIDOID);
        query.assertColumnOfType(3, PostgreSQL::INT8OID);
        query.assertColumnOfType(4, PostgreSQL::BPCHAROID);
        query.assertColumnOfType(5, PostgreSQL::FLOAT4OID);
        query.assertColumnOfType(6, PostgreSQL::VARCHAROID);

        this->timestamp         = query.popTIMESTAMP();
        this->thermaId          = query.popBIGINT();
        this->token             = query.popUUID();
        this->servusId          = query.popBIGINT();
        this->gpioDeviceNumber  = query.popCHAR();
        this->temperatureEdge   = query.popREAL();
        this->title             = query.popVARCHAR();
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot load DS18B20/DS18S20 sensor: %s",
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
Database::Therma::setTitle(const std::string& title)
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Sensors);

    try
    {
        std::unique_lock<std::mutex> queueLock { database.lock };

        PostgreSQL::Transaction transaction(database.connection());

        {
            PostgreSQL::Query query(database.connection());

            unsigned long thermaIdQuery = htobe64(this->thermaId);

            query.pushBIGINT(&thermaIdQuery);
            query.pushVARCHAR(&title);
            query.execute(QueryUpdateDSSensorTitle);

            query.assertNumberOfRows(1);
            query.assertNumberOfColumns(1);
            query.assertColumnOfType(0, PostgreSQL::VARCHAROID);

            this->title = query.popVARCHAR();
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot update DS18B20/DS18S20 sensor: %s",
                exception.what());

        throw exception;
    }
}

float
Database::Therma::lastKnownTemperature()
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Sensors);

    try
    {
        std::unique_lock<std::mutex> queueLock { database.lock };

        PostgreSQL::Transaction transaction(database.connection());

        {
            PostgreSQL::Query query(database.connection());

            unsigned long thermaIdQuery = htobe64(this->thermaId);

            query.pushBIGINT(&thermaIdQuery);
            query.execute(QueryDSSensorLastKnownTemperature);

            query.assertNumberOfRows(1);
            query.assertNumberOfColumns(1);
            query.assertColumnOfType(0, PostgreSQL::FLOAT4OID);

            return query.popREAL();
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot fetch data of DS18B20/DS18S20 sensor: %s",
                exception.what());

        throw exception;
    }
}

float
Database::Therma::lowestKnownTemperature()
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Sensors);

    try
    {
        std::unique_lock<std::mutex> queueLock { database.lock };

        PostgreSQL::Transaction transaction(database.connection());

        {
            PostgreSQL::Query query(database.connection());

            unsigned long thermaIdQuery = htobe64(this->thermaId);

            query.pushBIGINT(&thermaIdQuery);
            query.execute(QueryDSSensorLowestTemperature);

            query.assertNumberOfRows(1);
            query.assertNumberOfColumns(1);
            query.assertColumnOfType(0, PostgreSQL::FLOAT4OID);

            return query.popREAL();
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot fetch data of DS18B20/DS18S20 sensor: %s",
                exception.what());

        throw exception;
    }
}

float
Database::Therma::highestKnownTemperature()
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Sensors);

    try
    {
        std::unique_lock<std::mutex> queueLock { database.lock };

        PostgreSQL::Transaction transaction(database.connection());

        {
            PostgreSQL::Query query(database.connection());

            unsigned long thermaIdQuery = htobe64(this->thermaId);

            query.pushBIGINT(&thermaIdQuery);
            query.execute(QueryDSSensorHighestTemperature);

            query.assertNumberOfRows(1);
            query.assertNumberOfColumns(1);
            query.assertColumnOfType(0, PostgreSQL::FLOAT4OID);

            return query.popREAL();
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot fetch data of DS18B20/DS18S20 sensor: %s",
                exception.what());

        throw exception;
    }
}

std::string
Database::Therma::diagramAsJava()
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Sensors);

    try
    {
        std::unique_lock<std::mutex> queueLock { database.lock };

        PostgreSQL::Transaction transaction(database.connection());

        {
            PostgreSQL::Query query(database.connection());

            unsigned long thermaIdQuery = htobe64(this->thermaId);

            query.pushBIGINT(&thermaIdQuery);
            query.execute(QueryDSSensorDiagramAsJava);

            query.assertNumberOfRows(1);
            query.assertNumberOfColumns(1);
            query.assertColumnOfType(0, PostgreSQL::TEXTOID);

            return query.popVARCHAR();
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot build diagram of DS18B20/DS18S20 sensor: %s",
                exception.what());

        throw exception;
    }
}
