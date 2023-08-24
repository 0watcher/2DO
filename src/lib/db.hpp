#pragma once

#include <SQLiteCpp/SQLiteCpp.h>

#include <string>
#include <type_traits>
#include <vector>

#include "result.hpp"

using stringmap = std::map<std::string, std::string>;
using stringpair = std::pair<std::string, std::string>;
using stringvec = std::vector<std::string>;
using String = std::string;

namespace twodo
{
    enum class DbError
    {
        DbError,
        OpenFailure,
        TableCreateFailure,
        InsertFailure,
        SelectFailure,
        UpdateFailure,
        TableDropFailure,
    };

    class Db
    {
    public:
        Db(const std::string& path) noexcept : m_db(path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {}

        Result<None, DbError> create_table(const String& table_name, const stringmap& column_names) noexcept;

        Result<None, DbError> drop_table(const String& table_name) noexcept;

        Result<None, DbError> insert_data(const String& table_name, const stringmap& values) noexcept;

        Result<None, DbError> update_data(const String& table_name, const stringpair& set, const stringpair& where) noexcept;

        Result<stringvec, DbError> select_data(const String& table_name, const stringvec& who, const stringpair& where) noexcept;

    private:
        SQLite::Database m_db;
    };
}  // namespace twodo