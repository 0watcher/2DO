#pragma once

#include <SQLiteCpp/SQLiteCpp.h>
#include <string>
#include <vector>
#include "result.hpp"

using stringmap = std::map<std::string, std::string>;
using stringpair = std::pair<std::string, std::string>;
using stringvec = std::vector<std::string>;
using String = std::string;

namespace twodo
{
    enum class DbErr
    {
        Error = 1,
        EmptyProps,
        IncompatibleNumberOfColumns
    };

    struct DbError
    {
        DbError(String sq_err_) : sq_err{sq_err_} {}
        DbError(DbErr db_err_) : cdb_err{db_err_} {}

        String sql_err() {  return sq_err; }
        DbErr db_err() {  return cdb_err; }
    private:
        String sq_err{};
        DbErr cdb_err{};
    };

    class Database
    {
    public:
        Database(const std::string& path) noexcept : m_db{path + ".db3", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE} {}

        Result<None, DbError> create_table(const String& table_name, const stringmap& column_names);
        Result<None, DbError> drop_table(const String& table_name);
        Result<None, DbError> insert_data(const String& table_name, const stringmap& values);
        Result<None, DbError> delete_data(const String& table_name, const stringpair& where);
        Result<None, DbError> update_data(const String& table_name, const stringpair& set, const stringpair& where);
        Result<stringvec, DbError> select_data(const String& table_name, const stringvec& who, const stringpair& where);
        bool is_table_empty(const String& table_name);
        

    private:
        SQLite::Database m_db;
    };
}  // namespace twodo