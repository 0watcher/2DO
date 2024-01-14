#pragma once

#include <SQLiteCpp/SQLiteCpp.h>
#include "Utils/result.hpp"
#include "Utils/type.hpp"

namespace twodoutils {
using Id = int;
using Attribute = String;
using UpdatedValue = String;
using AttributeType = String;
using Value = String;
using Condition = std::pair<Attribute, Value>;

enum class DbErr {
    Error = 1,
    EmptyProps,
    IncompatibleNumberOfColumns,
    EmptyResult
};

class [[nodiscard]] DbError {
  public:
    DbError(String sq_err_) : sq_err{sq_err_} {}
    DbError(DbErr db_err_) : cdb_err{db_err_} {}

    String sql_err() const { return sq_err; }
    DbErr db_err() const { return cdb_err; }

  private:
    String sq_err{};
    DbErr cdb_err{};
};

class [[nodiscard]] Database {
  public:
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    Database(Database&& other) = default;
    Database& operator=(Database&& other) = default;

    Database(const String& path) noexcept
        : m_db{path + ".db3", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE} {}

    Result<None, DbError> create_table(
        const String& table_name,
        const HashMap<Attribute, AttributeType>& column_def);

    Result<None, DbError> drop_table(const String& table_name);

    Result<None, DbError> insert_data(const String& table_name,
                                      const HashMap<Attribute, Value>& values);
    
    Result<None, DbError> delete_data(const String& table_name,
                                      const Condition& where);
    
    Result<None, DbError> update_data(
        const String& table_name,
        const std::pair<Attribute, UpdatedValue>& set,
        const Condition& where);
    
    [[nodiscard]] Result<Vector<Value>, DbError> select_data (
        const String& table_name,
        const Vector<Attribute>& who,
        const Condition& where) const;
    
    [[nodiscard]] bool is_table_empty(const String& table_name) const;

  private:
    SQLite::Database m_db;
};
}  // namespace twodoutils
