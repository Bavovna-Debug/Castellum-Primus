// System definition files.
//
#include <endian.h>
#include <cstring>

// Common definition files.
//
#include "PostgreSQL/PostgreSQL.hpp"
#include "Toolkit/Report.h"
#include "Toolkit/Times.hpp"

// Local definition files.
//
#include "Primus/Database/Database.hpp"
#include "Primus/Database/DHTSensor.hpp"
#include "Primus/Database/DHTSensorList.hpp"
#include "Primus/Database/Queries/DHT.h"

unsigned long
Database::DHTSensorList::TotalNumber()
{
    unsigned long numberOfSensors;

    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        std::unique_lock<std::mutex> queueLock { database.lock };

        PostgreSQL::Query query(database.connection());

        query.execute(QueryTotalNumberOfDHTSensors);

        query.assertNumberOfRows(1);
        query.assertNumberOfColumns(1);
        query.assertColumnOfType(0, PostgreSQL::INT8OID);

        numberOfSensors = query.popBIGINT();
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot get number of DHT11/DHT22 sensors: %s",
                exception.what());

        throw exception;
    }

    return numberOfSensors;
}

Database::DHTSensor&
Database::DHTSensorList::SensorByIndex(const unsigned long thermaIndex)
{
    unsigned long sensorId;

    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        std::unique_lock<std::mutex> queueLock { database.lock };

        PostgreSQL::Query query(database.connection());

        unsigned long thermaIndexQuery = htobe64(thermaIndex);

        query.pushBIGINT(&thermaIndexQuery);
        query.execute(QuerySearchForDHTSensorByIndex);

        query.assertNumberOfRows(1);
        query.assertNumberOfColumns(1);
        query.assertColumnOfType(0, PostgreSQL::INT8OID);

        sensorId = query.popBIGINT();
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot find DHT11/DHT22 sensor: %s",
                exception.what());

        throw exception;
    }

    return Database::DHTSensorList::SensorById(sensorId);
}

Database::DHTSensor&
Database::DHTSensorList::SensorById(const unsigned long sensorId)
{
    Database::DHTSensor* sensor = new Database::DHTSensor(sensorId);

    return *sensor;
}

void
Database::NoticeDHTSensorHumidity(
    Toolkit::Timestamp& originTimestamp,
    const std::string&  sensorToken,
    const double        stampAsReal,
    const float         humidity)
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        std::unique_lock<std::mutex> queueLock { database.lock };

        PostgreSQL::Transaction transaction(database.connection());

        {
            PostgreSQL::Query query(database.connection());

            union
            {
                unsigned long   stampInteger;
                double          stampReal;
            };

            memcpy(&stampInteger, &stampAsReal, sizeof(stampAsReal));
            stampInteger = htobe64(stampInteger);

            union
            {
                unsigned int    humidityInteger;
                float           humidityReal;
            };

            memcpy(&humidityInteger, &humidity, sizeof(humidity));
            humidityInteger = htobe32(humidityInteger);

            query.pushTIMESTAMP(originTimestamp);
            query.pushUUID(&sensorToken);
            query.pushDOUBLE(&stampReal);
            query.pushREAL(&humidityReal);
            query.execute(QueryInsertDHTSensorHumidity);
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot store DHT11/DHT22 sensor humidity: %s",
                exception.what());

        throw exception;
    }
}

void
Database::NoticeDHTSensorTemperature(
    Toolkit::Timestamp& originTimestamp,
    const std::string&  sensorToken,
    const double        stampAsReal,
    const float         temperature)
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        std::unique_lock<std::mutex> queueLock { database.lock };

        PostgreSQL::Transaction transaction(database.connection());

        {
            PostgreSQL::Query query(database.connection());

            union
            {
                unsigned long   stampInteger;
                double          stampReal;
            };

            memcpy(&stampInteger, &stampAsReal, sizeof(stampAsReal));
            stampInteger = htobe64(stampInteger);

            union
            {
                unsigned int    temperatureInteger;
                float           temperatureReal;
            };

            memcpy(&temperatureInteger, &temperature, sizeof(temperature));
            temperatureInteger = htobe32(temperatureInteger);

            query.pushTIMESTAMP(originTimestamp);
            query.pushUUID(&sensorToken);
            query.pushDOUBLE(&stampReal);
            query.pushREAL(&temperatureReal);
            query.execute(QueryInsertDHTSensorTemperature);
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot store DHT11/DHT22 sensor temperature: %s",
                exception.what());

        throw exception;
    }
}
