#pragma once

#include <SQLiteCpp/SQLiteCpp.h>

#include <string>
#include <type_traits>
#include <vector>

#include "result.hpp"

using stringmap = std::map<std::string, std::string>;
using stringpair = std::pair<std::string, std::string>;

namespace twodo
{
class DB
{
   public:
    DB(const std::string& path) : m_db(path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {}

    Result<None, DbError> create_table(const std::string& table_name,
                                       const stringmap& column_names);

    Result<None, DbError> drop_table(const std::string& table_name);

    Result<None, DbError> insert_data(const std::string& table_name, const stringmap& values);

    Result<None, DbError> update_data(const std::string& table_name, const stringpair& set,
                                      const stringpair& where);

    Result<std::vector<std::string>, DbError> select_data(const std::string& table_name,
                                                          const stringpair& where);

   private:
    SQLite::Database m_db;
};
}  // namespace twodo