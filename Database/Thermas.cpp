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
#include "Primus/Database/Therma.hpp"
#include "Primus/Database/Thermas.hpp"
#include "Primus/Database/Queries/Therma.h"

unsigned long
Database::Thermas::TotalNumber()
{
    unsigned long numberOfThermas;

    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Sensors);

    try
    {
        std::unique_lock<std::mutex> queueLock { database.lock };

        PostgreSQL::Query query(database.connection());

        query.execute(QueryTotalNumberOfDSSensors);

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
        ReportError("[Database] Cannot get number of DS18B20/DS18S20 sensors: %s",
                exception.what());

        throw exception;
    }

    return numberOfThermas;
}

Database::Therma&
Database::Thermas::SensorByIndex(const unsigned long sensorId)
{
    unsigned long thermaId;

    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Sensors);

    try
    {
        std::unique_lock<std::mutex> queueLock { database.lock };

        PostgreSQL::Query query(database.connection());

        unsigned long sensorIdQuery = htobe64(sensorId);

        query.pushBIGINT(&sensorIdQuery);
        query.execute(QuerySearchForDSSensorByIndex);

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
        ReportError("[Database] Cannot find DS18B20/DS18S20 sensor: %s",
                exception.what());

        throw exception;
    }

    return Database::Thermas::SensorById(thermaId);
}

Database::Therma&
Database::Thermas::SensorById(const unsigned long sensorId)
{
    Database::Therma* therma = new Database::Therma(sensorId);

    return *therma;
}

void
Database::NoticeDSSensorTemperature(
    Toolkit::Timestamp& originTimestamp,
    const std::string&  sensorToken,
    const double        stampAsReal,
    const float         temperature)
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Sensors);

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
            query.execute(QueryInsertDSSensorTemperature);
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot store DS18B20/DS18S20 sensor temperature: %s",
                exception.what());

        throw exception;
    }
}
