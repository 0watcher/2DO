#pragma once

#include <SQLiteCpp/SQLiteCpp.h>

#include <iostream>
#include <string>
#include <vector>

#include "result.hpp"

using String = std::string;
using ColumnDef = std::map<std::string, std::string>;
using ColumnNames = std::vector<std::string>;
using ColumnInsert = std::map<std::string, std::string>;
using ColumnUpdate = std::pair<std::string, std::string>;
using Condition = std::pair<std::string, std::string>;
using ColumnValues = std::vector<std::string>;

namespace twodo
{
[[nodiscard]] String input();

[[nodiscard]] String hash(const String& str);

class IUserInputHandler
{
   public:
    virtual String get_input() = 0;
    virtual ~IUserInputHandler() = default;
};

class IDisplayer
{
   public:
    virtual void msg_display(std::string_view msg) = 0;
    virtual void err_display(std::string_view err) = 0;
    virtual ~IDisplayer() = default;
};

enum class DbErr
{
    Error = 1,
    EmptyProps,
    IncompatibleNumberOfColumns,
    EmptyResult
};

struct DbError
{
    DbError(String sq_err_) : sq_err {sq_err_} {}
    DbError(DbErr db_err_) : cdb_err {db_err_} {}

    String sql_err() const { return sq_err; }
    DbErr db_err() const { return cdb_err; }

   private:
    String sq_err {};
    DbErr cdb_err {};
};

class Database
{
   public:
    Database(const String& path) noexcept
        : m_db {path + ".db3", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE} {}

    Result<None, DbError> create_table(const String& table_name, const ColumnDef& column_names);
    Result<None, DbError> drop_table(const String& table_name);
    Result<None, DbError> insert_data(const String& table_name, const ColumnInsert& values);
    Result<None, DbError> delete_data(const String& table_name, const Condition& where);
    Result<None, DbError> update_data(const String& table_name, const ColumnUpdate& set,
                                      const Condition& where);
    [[nodiscard]] Result<ColumnValues, DbError> select_data(const String& table_name, const ColumnNames& who,
                                              const Condition& where);
    [[nodiscard]] bool is_table_empty(const String& table_name);

   private:
    SQLite::Database m_db;
};
}  // namespace twodo
