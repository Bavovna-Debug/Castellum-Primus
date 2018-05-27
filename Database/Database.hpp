#pragma once

// Common definition files.
//
#include "PostgreSQL/Exception.hpp"
#include "PostgreSQL/PostgreSQL.hpp"

namespace Primus
{
    class Database
    {
    public:
        enum Instance
        {
            Default,
            Fabulatorium,
            Notificator,
            WWW,
            NumberOfInstances
        };

    public:
        PostgreSQL::Connection* connection;

    public:
        static Database&
        InitInstance(const Primus::Database::Instance);

        static Database&
        SharedInstance(const Primus::Database::Instance);

        Database();

        ~Database();

        void
        connect(),
        disconnect(),
        recover(PostgreSQL::OperatorIntervention&);
    };
};
