// System definition files.
//
#include <endian.h>
#include <cstring>
#include <mutex>
#include <stdexcept>
#include <system_error>

// Common definition files.
//
#include "PostgreSQL/Exception.hpp"
#include "PostgreSQL/PostgreSQL.hpp"
#include "RTSP/RTSP.hpp"
#include "Toolkit/Report.h"

// Local definition files.
//
#include "Primus/Configuration.hpp"
#include "Primus/Database/Debug.hpp"
#include "Primus/Database/Queries/Debug.h"

static Primus::Debug* instance = NULL;

Primus::Debug&
Primus::Debug::InitInstance()
{
    if (instance != NULL)
        throw std::runtime_error("Debug already initialized");

    instance = new Primus::Debug();

    return *instance;
}

Primus::Debug&
Primus::Debug::SharedInstance()
{
    if (instance == NULL)
        throw std::runtime_error("Debug not initialized");

    return *instance;
}

Primus::Debug::Debug()
{
    this->connect();
}

Primus::Debug::~Debug()
{ }

void
Primus::Debug::connect()
{
    Primus::Configuration& configuration = Primus::Configuration::SharedInstance();

    this->connection = new PostgreSQL::Connection(
            configuration.database.hostName.c_str(),
            configuration.database.portNumber,
            configuration.database.databaseName.c_str(),
            configuration.database.role.c_str(),
            configuration.database.password.c_str());
}

void
Primus::Debug::disconnect()
{
    delete this->connection;
}

void
Primus::Debug::recover(PostgreSQL::OperatorIntervention& exception)
{
    if (!exception.queryCanceled())
    {
        this->disconnect();
        this->connect();
    }
}

unsigned long
Primus::Debug::BeginServusSession(const std::string& servusIP)
{
    Primus::Debug& debug = Primus::Debug::SharedInstance();

    std::unique_lock<std::mutex> queueLock { debug.lock };

    unsigned long sessionId = 0;

    try
    {
        PostgreSQL::Transaction transaction(*debug.connection);

        {
            PostgreSQL::Query query(*debug.connection);

            query.pushINET(servusIP.c_str(), servusIP.length());

            query.execute(QueryBeginServusSession);

            query.assertNumberOfRows(1);
            query.assertNumberOfColumns(1);
            query.assertColumnOfType(0, PostgreSQL::INT8OID);

            sessionId = query.popBIGINT();
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        debug.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportWarning("[Debug] PostgreSQL exception: %s", exception.what());
    }

    return sessionId;
}

void
Primus::Debug::CloseServusSession(const unsigned long sessionId)
{
    Primus::Debug& debug = Primus::Debug::SharedInstance();

    std::unique_lock<std::mutex> queueLock { debug.lock };

    try
    {
        PostgreSQL::Transaction transaction(*debug.connection);

        {
            PostgreSQL::Query query(*debug.connection);

            unsigned long sessionIdQuery = htobe64(sessionId);

            query.pushBIGINT(&sessionIdQuery);

            query.execute(QueryCloseServusSession);
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        debug.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportWarning("[Debug] PostgreSQL exception: %s", exception.what());
    }
}

void
Primus::Debug::CommentServusSession(
    const unsigned long sessionId,
    const std::string&  comment)
{
    Primus::Debug& debug = Primus::Debug::SharedInstance();

    std::unique_lock<std::mutex> queueLock { debug.lock };

    try
    {
        PostgreSQL::Transaction transaction(*debug.connection);

        {
            PostgreSQL::Query query(*debug.connection);

            unsigned long sessionIdQuery = htobe64(sessionId);

            query.pushBIGINT(&sessionIdQuery);
            query.pushVARCHAR(&comment);

            query.execute(QueryCommentServusSession);
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        debug.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportWarning("[Debug] PostgreSQL exception: %s", exception.what());
    }
}

void
Primus::Debug::ReportServusRTSP(
    const unsigned long sessionId,
    RTSP::Datagram&     request,
    RTSP::Datagram&     response)
{
    Primus::Debug& debug = Primus::Debug::SharedInstance();

    std::unique_lock<std::mutex> queueLock { debug.lock };

    try
    {
        PostgreSQL::Transaction transaction(*debug.connection);

        {
            PostgreSQL::Query query(*debug.connection);

            unsigned long sessionIdQuery = htobe64(sessionId);

            query.pushBIGINT(&sessionIdQuery);
            query.pushVARCHAR(request.contentBuffer, request.contentLength);
            query.pushVARCHAR(response.contentBuffer, response.contentLength);

            query.execute(QueryReportServusRTSP);
        }
    }
    catch (PostgreSQL::DataException& exception)
    {
        if (exception.characterNotInRepertoire())
        {
            ReportWarning("[Debug] RTSP query contains bad characters");
        }
        else
        {
            ReportWarning("[Debug] PostgreSQL exception: %s", exception.what());
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        debug.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportWarning("[Debug] PostgreSQL exception: %s", exception.what());
    }
}

unsigned long
Primus::Debug::BeginPhoenixSession(const std::string& phoenixIP)
{
    Primus::Debug& debug = Primus::Debug::SharedInstance();

    std::unique_lock<std::mutex> queueLock { debug.lock };

    unsigned long sessionId = 0;

    try
    {
        PostgreSQL::Transaction transaction(*debug.connection);

        {
            PostgreSQL::Query query(*debug.connection);

            query.pushINET(phoenixIP.c_str(), phoenixIP.length());

            query.execute(QueryBeginPhoenixSession);

            query.assertNumberOfRows(1);
            query.assertNumberOfColumns(1);
            query.assertColumnOfType(0, PostgreSQL::INT8OID);

            sessionId = query.popBIGINT();
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        debug.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportWarning("[Debug] PostgreSQL exception: %s", exception.what());
    }

    return sessionId;
}

void
Primus::Debug::ClosePhoenixSession(const unsigned long sessionId)
{
    Primus::Debug& debug = Primus::Debug::SharedInstance();

    std::unique_lock<std::mutex> queueLock { debug.lock };

    try
    {
        PostgreSQL::Transaction transaction(*debug.connection);

        {
            PostgreSQL::Query query(*debug.connection);

            unsigned long sessionIdQuery = htobe64(sessionId);

            query.pushBIGINT(&sessionIdQuery);

            query.execute(QueryClosePhoenixSession);
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        debug.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportWarning("[Debug] PostgreSQL exception: %s", exception.what());
    }
}

void
Primus::Debug::CommentPhoenixSession(
    const unsigned long sessionId,
    const std::string&  comment)
{
    Primus::Debug& debug = Primus::Debug::SharedInstance();

    std::unique_lock<std::mutex> queueLock { debug.lock };

    try
    {
        PostgreSQL::Transaction transaction(*debug.connection);

        {
            PostgreSQL::Query query(*debug.connection);

            unsigned long sessionIdQuery = htobe64(sessionId);

            query.pushBIGINT(&sessionIdQuery);
            query.pushVARCHAR(&comment);

            query.execute(QueryCommentPhoenixSession);
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        debug.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportWarning("[Debug] PostgreSQL exception: %s", exception.what());
    }
}

void
Primus::Debug::ReportPhoenixRTSP(
    const unsigned long sessionId,
    RTSP::Datagram&     request,
    RTSP::Datagram&     response)
{
    Primus::Debug& debug = Primus::Debug::SharedInstance();

    std::unique_lock<std::mutex> queueLock { debug.lock };

    try
    {
        PostgreSQL::Transaction transaction(*debug.connection);

        {
            PostgreSQL::Query query(*debug.connection);

            unsigned long sessionIdQuery = htobe64(sessionId);
            unsigned int responseStatus = htobe32(response.statusCode);

            query.pushBIGINT(&sessionIdQuery);
            query.pushVARCHAR(request.contentBuffer, request.contentLength);
            query.pushVARCHAR(response.contentBuffer, response.contentLength);
            query.pushINTEGER(&responseStatus);

            query.execute(QueryReportPhoenixRTSP);
        }
    }
    catch (PostgreSQL::OperatorIntervention& exception)
    {
        debug.recover(exception);

        throw exception;
    }
    catch (PostgreSQL::Exception& exception)
    {
        ReportWarning("[Debug] PostgreSQL exception: %s", exception.what());
    }
}
