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
#include "Primus/Database/Relay.hpp"
#include "Primus/Database/Relays.hpp"
#include "Primus/Database/Queries/Relay.h"

unsigned long
Database::Relays::TotalNumber()
{
    unsigned long numberOfRelays;

    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Sensors);

    try
    {
        std::unique_lock<std::mutex> queueLock { database.lock };

        PostgreSQL::Query query(database.connection());

        query.execute(QueryTotalNumberOfRelays);

        query.assertNumberOfRows(1);
        query.assertNumberOfColumns(1);
        query.assertColumnOfType(0, PostgreSQL::INT8OID);

        numberOfRelays = query.popBIGINT();
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot get number of relays: %s",
                exception.what());

        throw exception;
    }

    return numberOfRelays;
}

Database::Relay&
Database::Relays::RelayByIndex(const unsigned long relayIndex)
{
    unsigned long relayId;

    Primus::Database& database = Primus::Database::SharedInstance(Primus::Database::Sensors);

    try
    {
        std::unique_lock<std::mutex> queueLock { database.lock };

        PostgreSQL::Query query(database.connection());

        unsigned long relayIndexQuery = htobe64(relayIndex);

        query.pushBIGINT(&relayIndexQuery);
        query.execute(QuerySearchForRelayByIndex);

        query.assertNumberOfRows(1);
        query.assertNumberOfColumns(1);
        query.assertColumnOfType(0, PostgreSQL::INT8OID);

        relayId = query.popBIGINT();
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        database.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportError("[Database] Cannot find relay: %s",
                exception.what());

        throw exception;
    }

    return Database::Relays::RelayById(relayId);
}

Database::Relay&
Database::Relays::RelayById(const unsigned long relayId)
{
    Database::Relay* relay = new Database::Relay(relayId);

    return *relay;
}
