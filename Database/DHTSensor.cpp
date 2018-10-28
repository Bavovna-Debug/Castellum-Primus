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
#include "Primus/Database/DHTSensor.hpp"
#include "Primus/Database/Queries/DHT.h"

Database::DHTSensor::DHTSensor(const unsigned long sensorId)
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Query query(*database.connection);

        unsigned long sensorIdQuery = htobe64(sensorId);

        query.pushBIGINT(&sensorIdQuery);
        query.execute(QuerySearchForDHTSensorById);

        query.assertNumberOfRows(1);
        query.assertNumberOfColumns(8);
        query.assertColumnOfType(0, PostgreSQL::TIMESTAMPOID);
        query.assertColumnOfType(1, PostgreSQL::INT8OID);
        query.assertColumnOfType(2, PostgreSQL::UUIDOID);
        query.assertColumnOfType(3, PostgreSQL::INT8OID);
        query.assertColumnOfType(4, PostgreSQL::INT2OID);
        query.assertColumnOfType(5, PostgreSQL::FLOAT4OID);
        query.assertColumnOfType(6, PostgreSQL::FLOAT4OID);
        query.assertColumnOfType(7, PostgreSQL::VARCHAROID);

        this->timestamp         = query.popTIMESTAMP();
        this->sensorId          = query.popBIGINT();
        this->token             = query.popUUID();
        this->servusId          = query.popBIGINT();
        this->gpioPinNumber     = query.popSMALL();
        this->humidityEdge      = query.popREAL();
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
        ReportError("[Database] Cannot load DHT11/DHT22 sensor: %s",
                exception.what());

        throw exception;
    }
}

Database::DHTSensor::~DHTSensor()
{
    if (this->timestamp != NULL)
    {
        delete this->timestamp;
    }
}

float
Database::DHTSensor::lastKnownHumidity()
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Transaction transaction(*database.connection);

        {
            PostgreSQL::Query query(*database.connection);

            unsigned long sensorIdQuery = htobe64(this->sensorId);

            query.pushBIGINT(&sensorIdQuery);
            query.execute(QueryDHTSensorLastKnownHumidity);

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
        ReportError("[Database] Cannot fetch data of DHT11/DHT22 sensor: %s",
                exception.what());

        throw exception;
    }
}

float
Database::DHTSensor::lowestKnownHumidity()
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Transaction transaction(*database.connection);

        {
            PostgreSQL::Query query(*database.connection);

            unsigned long sensorIdQuery = htobe64(this->sensorId);

            query.pushBIGINT(&sensorIdQuery);
            query.execute(QueryDHTSensorLowestHumidity);

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
        ReportError("[Database] Cannot fetch data of DHT11/DHT22 sensor: %s",
                exception.what());

        throw exception;
    }
}

float
Database::DHTSensor::highestKnownHumidity()
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Transaction transaction(*database.connection);

        {
            PostgreSQL::Query query(*database.connection);

            unsigned long sensorIdQuery = htobe64(this->sensorId);

            query.pushBIGINT(&sensorIdQuery);
            query.execute(QueryDHTSensorHighestHumidity);

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
        ReportError("[Database] Cannot fetch data of DHT11/DHT22 sensor: %s",
                exception.what());

        throw exception;
    }
}

float
Database::DHTSensor::lastKnownTemperature()
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Transaction transaction(*database.connection);

        {
            PostgreSQL::Query query(*database.connection);

            unsigned long sensorIdQuery = htobe64(this->sensorId);

            query.pushBIGINT(&sensorIdQuery);
            query.execute(QueryDHTSensorLastKnownTemperature);

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
        ReportError("[Database] Cannot fetch data of DHT11/DHT22 sensor: %s",
                exception.what());

        throw exception;
    }
}

float
Database::DHTSensor::lowestKnownTemperature()
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Transaction transaction(*database.connection);

        {
            PostgreSQL::Query query(*database.connection);

            unsigned long sensorIdQuery = htobe64(this->sensorId);

            query.pushBIGINT(&sensorIdQuery);
            query.execute(QueryDHTSensorLowestTemperature);

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
        ReportError("[Database] Cannot fetch data of DHT11/DHT22 sensor: %s",
                exception.what());

        throw exception;
    }
}

float
Database::DHTSensor::highestKnownTemperature()
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Transaction transaction(*database.connection);

        {
            PostgreSQL::Query query(*database.connection);

            unsigned long sensorIdQuery = htobe64(this->sensorId);

            query.pushBIGINT(&sensorIdQuery);
            query.execute(QueryDHTSensorHighestTemperature);

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
        ReportError("[Database] Cannot fetch data of DHT11/DHT22 sensor: %s",
                exception.what());

        throw exception;
    }
}
