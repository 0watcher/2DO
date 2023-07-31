#pragma once

#include <string>
#include <vector>
#include <sqlite3.h>

#include "result.hpp"

namespace twodo
{
    class sql_ptr
    {
    public:
        sql_ptr() : m_db(nullptr, [](sqlite3* ptr) { if (ptr) sqlite3_close_v2(ptr); }) {}

        sqlite3* get() const {
            return m_db.get();
        }

        void reset(sqlite3* newPtr = nullptr) {
            m_db.reset(newPtr, [](sqlite3* ptr) { if (ptr) sqlite3_close_v2(ptr); });
        }

        sqlite3* operator->() const {
            return m_db.get();
        }

        operator bool() const {
            return m_db.get() != nullptr;
        }

    private:
        std::shared_ptr<sqlite3> m_db;
    };

    class Database
    {
    public:
        Database() = default;

        Result<sql_ptr> create_or_open(const char* path) const;
    };
}