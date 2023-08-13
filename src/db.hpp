#pragma once

#include <SQLiteCpp/SQLiteCpp.h>
#include "result.hpp"

#include <string>
#include <type_traits>
#include <vector>

namespace twodo
{
    class DB
    {
    public:
        DB(const std::string& path) 
        : m_db(path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {}

        Result<None, DbError> create_table(const std::string& table_name,
                                  const std::map<std::string, std::string>& column_names);

        Result<None, DbError> drop_table(const std::string& table_name);

        Result<None, DbError> insert_data(const std::string& table_name,
                                 const std::map<std::string, std::string>& values);

        Result<None, DbError> update_data(const std::string& table_name, 
                                 const std::pair<std::string, std::string>& set, 
                                 const std::pair<std::string, std::string>& where);

        Result<std::vector<std::string>, DbError> select_data(const std::string& table_name,
                                 const std::pair<std::string, std::string>& where);

    private:
        SQLite::Database m_db;
    };
}