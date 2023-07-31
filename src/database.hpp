#pragma once

#include <string>
#include <vector>
#include <memory>
#include <sqlite3.h>

#include "result.hpp"

namespace twodo
{
    struct sql3_closer
    {
        void operator()(sqlite3* p) const
        {
            sqlite3_close_v2(p);
        }
    };

    using sql3_ptr = std::unique_ptr<sqlite3, sql3_closer>;

    class Database
    {
    public:
        Database() = default;

        Result<sql3_ptr> create_or_open(const char* path) const;
        Result<void> create_table(const sql3_ptr db, const char* table);

    };
}