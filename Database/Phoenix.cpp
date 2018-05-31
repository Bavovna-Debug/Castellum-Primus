// System definition files.
//
#include <string>

// Common definition files.
//
#include "APNS/APNS.hpp"
#include "PostgreSQL/PostgreSQL.hpp"
#include "Toolkit/Report.h"
#include "Toolkit/Times.hpp"

// Local definition files.
//
#include "Primus/Database/Database.hpp"
#include "Primus/Database/Phoenix.hpp"
#include "Primus/Database/Queries/Phoenix.h"

Database::Phoenix::Phoenix(const unsigned long phoenixId)
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Query query(*database.connection);

        unsigned long phoenixIdQuery = htobe64(phoenixId);

        query.pushBIGINT(&phoenixIdQuery);
        query.execute(QuerySearchForPhoenixById);

        query.assertNumberOfRows(1);
        query.assertNumberOfColumns(7);
        query.assertColumnOfType(0, PostgreSQL::TIMESTAMPOID);
        query.assertColumnOfType(1, PostgreSQL::INT8OID);
        query.assertColumnOfType(2, PostgreSQL::UUIDOID);
        query.assertColumnOfType(3, PostgreSQL::VARCHAROID);
        query.assertColumnOfType(4, PostgreSQL::VARCHAROID);
        query.assertColumnOfType(5, PostgreSQL::VARCHAROID);
        query.assertColumnOfType(6, PostgreSQL::VARCHAROID);

        this->timestamp         = query.popTIMESTAMP();
        this->phoenixId         = query.popBIGINT();
        this->token             = query.popUUID();
        this->deviceName        = query.popVARCHAR();
        this->deviceModel       = query.popVARCHAR();
        this->softwareVersion   = query.popVARCHAR();
        this->description       = query.popVARCHAR();
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot load phoenix: %s",
                exception.what());

        throw exception;
    }
}

Database::Phoenix::~Phoenix()
{
    if (this->timestamp != NULL)
    {
        delete this->timestamp;
    }
}

void
Database::Phoenix::saveDeviceToken()
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Transaction transaction(*database.connection);

        {
            PostgreSQL::Query query(*database.connection);

            unsigned long phoenixIdQuery = htobe64(this->phoenixId);

            query.pushBIGINT(&phoenixIdQuery);
            query.pushBYTEA((char*) &this->deviceToken, APNS::DeviceTokenLength);
            query.execute(QueryUpdatePhoenixDeviceToken);
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot update phoenix: %s",
                exception.what());

        throw exception;
    }
}

void
Database::Phoenix::setDescription(const std::string& description)
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Transaction transaction(*database.connection);

        {
            PostgreSQL::Query query(*database.connection);

            unsigned long phoenixIdQuery = htobe64(this->phoenixId);

            query.pushBIGINT(&phoenixIdQuery);
            query.pushVARCHAR(&description);
            query.execute(QueryUpdatePhoenixDescription);

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
        ReportError("[Database] Cannot update phoenix: %s",
                exception.what());

        throw exception;
    }
}

void
Database::Phoenix::setSoftwareVersion(const std::string& softwareVersion)
{
    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Transaction transaction(*database.connection);

        {
            PostgreSQL::Query query(*database.connection);

            unsigned long phoenixIdQuery = htobe64(this->phoenixId);

            query.pushBIGINT(&phoenixIdQuery);
            query.pushVARCHAR(&softwareVersion);
            query.execute(QueryUpdatePhoenixSoftwareVersion);

            query.assertNumberOfRows(1);
            query.assertNumberOfColumns(1);
            query.assertColumnOfType(0, PostgreSQL::VARCHAROID);

            this->softwareVersion = query.popVARCHAR();
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot update phoenix: %s",
                exception.what());

        throw exception;
    }
}

unsigned long
Database::Phoenix::RegisterPhoenixWithActivationCode(
    const std::string&  activationCode,
    const std::string&  vendorToken,
    const std::string&  deviceName,
    const std::string&  deviceModel,
    const std::string&  softwareVersion,
    const std::string&  description)
{
    unsigned long phoenixId;

    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Default);

    try
    {
        PostgreSQL::Transaction transaction(*database.connection);

        // Do all the following steps in one transaction to guarantee data integrity.
        //
        try
        {
            unsigned long activatorId;

            // Search for activator with specified activation code.
            // Throw exception if specified activation code does not exist.
            //
            {
                PostgreSQL::Query query(*database.connection);

                query.pushVARCHAR(&activationCode);
                query.execute(QuerySearchForActivatorByCode);

                query.assertNumberOfRows(1); /**< This may cause an exception. */
                query.assertNumberOfColumns(1);
                query.assertColumnOfType(0, PostgreSQL::INT8OID);

                activatorId = query.popBIGINT();
            }

            // Create a new record for new phoenix.
            //
            {
                PostgreSQL::Query query(*database.connection);

                query.pushUUID(&vendorToken);
                query.pushVARCHAR(&deviceName);
                query.pushVARCHAR(&deviceModel);
                query.pushVARCHAR(&softwareVersion);
                query.pushVARCHAR(&description);
                query.execute(QueryInsertPhoenix);

                query.assertNumberOfRows(1);
                query.assertNumberOfColumns(1);
                query.assertColumnOfType(0, PostgreSQL::INT8OID);

                phoenixId = query.popBIGINT();
            }

            // Associate the activator record with the corresponding phoenix.
            // Since then this activator is interpreted as 'used'. This guarantees
            // that the same activator would not be used to activate more than one phoenix.
            //
            {
                PostgreSQL::Query query(*database.connection);

                unsigned long activatorIdQuery = htobe64(activatorId);
                unsigned long phoenixIdQuery = htobe64(phoenixId);

                query.pushBIGINT(&activatorIdQuery);
                query.pushBIGINT(&phoenixIdQuery);
                query.execute(QuaraAssociateActivatorWithPhoenix);
            }
        }
        catch (PostgreSQL::UnexpectedQueryResponse&)
        {
            // This exception is expected in case the activator with the specified activation code
            // either does not exist or has been already used by some another phoenix.

            return 0;
        }
        catch (PostgreSQL::Exception &exception)
        {
            transaction.abandon();

            throw exception;
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot register phoenix: %s",
                exception.what());

        throw exception;
    }

    return phoenixId;
}
