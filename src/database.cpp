#include "database.hpp"

namespace twodo
{
    Result<sql3_ptr> Database::create_or_open(const std::string& path) const
    {
        sqlite3 *buffer = nullptr;
        auto result = sqlite3_open(path.c_str(), &buffer);
        if (!result)
        {
            return Result<sql3_ptr>{.m_err = ErrorCode::db_failure};
        }

        return Result<sql3_ptr>(sql3_ptr(buffer));
    }

    Result<std::string> Database::create_table(const sql3_ptr& db,
                                        const std::string& table_name, 
                                        const std::map<std::string, std::string> &column_names)
    {
        std::string sql_table = "CREATE TABLE IF NOT EXIST " + table_name + " (";

        for(const auto& column : column_names)
        {
            sql_table += column.first + " " + column.second + ", ";
        }

        if(!column_names.empty())
        {
            sql_table.pop_back();
        }

        sql_table += ");";
        
        char* err_msg = nullptr;
        auto result = sqlite3_exec(db.get(), sql_table.c_str(), nullptr, nullptr, &err_msg);

        auto err_msg_str = std::string(err_msg);

        sqlite3_free(err_msg);

        if(!result)
        {
            return Result<std::string>{ err_msg_str, ErrorCode::db_failure };
        }

        return Result<std::string>();
    }

    Result<std::string> Database::insert_data(const sql3_ptr &db, 
                                       const std::string &table_name, 
                                       const std::map<std::string, std::string> &values)
    {
        std::string sql_insert = "INSERT INTO " + table_name + + " (";

        for(const auto& column : values)
        {
            sql_insert += column.first + ",";
        }

        sql_insert.pop_back();
        sql_insert += ") VALUES ";

        sql_insert += "(";
        for(const auto& value : values)
        {
            sql_insert += "'" + value.second + "',";
        }
        sql_insert.pop_back();
        sql_insert += ");";

        char* err_msg = nullptr;
        auto result = sqlite3_exec(db.get(), sql_insert.c_str(), nullptr, nullptr, &err_msg);
        auto err_msg_str = std::string(err_msg);
        sqlite3_free(err_msg);

        if(!result)
        {
            return Result<std::string>{ err_msg_str, ErrorCode::db_failure };
        }
        return Result<std::string>();
    }

    Result<std::string> Database::select_data(const sql3_ptr &db, 
                                       const std::string &table_name, 
                                       const std::pair<std::string, std::string>& where)
    {
        std::string sql_select
        = "SELECT * FROM " + table_name + "WHERE " + where.first + " = " + where.second + ";";

        char* err_msg = nullptr;
        auto result = sqlite3_exec(db.get(), sql_select.c_str(), nullptr, nullptr, &err_msg);
        auto err_msg_str = std::string(err_msg);
        sqlite3_free(err_msg);

        if(!result)
        {
            return Result<std::string>{ err_msg_str, ErrorCode::db_failure };
        }

        return Result<std::string>();
    }

    Result<std::string> Database::update_data(const sql3_ptr &db, 
                                       const std::string &table_name, 
                                       const std::pair<std::string, std::string>& set, 
                                       const std::pair<std::string, std::string>& where)
    {
        std::string sql_update
        = "UPDATE " + table_name + " SET " + set.first + " = " + set.second + " WHERE " + where.first + " = " + "'" + where.second + "'" ";";
        
        char* err_msg = nullptr;
        auto result = sqlite3_exec(db.get(), sql_update.c_str(), nullptr, nullptr, &err_msg);
        auto err_msg_str = std::string(err_msg);
        sqlite3_free(err_msg);

        if(!result)
        {
            return Result<std::string>{ err_msg_str, ErrorCode::db_failure };
        }
        return Result<std::string>();
    }
}