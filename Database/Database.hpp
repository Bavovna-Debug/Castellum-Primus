#pragma once

// System definition files.
//
#include <mutex>

// Common definition files.
//
#include "PostgreSQL/Exception.hpp"
#include "PostgreSQL/PostgreSQL.hpp"

namespace Primus
{
    class Database
    {
    private:
        PostgreSQL::Connection* connectionInstance;

    public:
        std::mutex lock;

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
        static Primus::Database&
        InitInstance(const Primus::Database::Instance);

        static Primus::Database&
        SharedInstance(const Primus::Database::Instance);

        Database();

        ~Database();

        void
        connect(),
        disconnect(),
        recover(PostgreSQL::OperatorIntervention&);

        PostgreSQL::Connection&
        connection()
        { return *this->connectionInstance; }
    };
};
