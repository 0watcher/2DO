#include "database.hpp"

namespace twodo
{
    Result<void> Database::create_or_open(const std::string& path)
    {
        sqlite3 *buffer = nullptr;
        auto result = sqlite3_open(path.c_str(), &buffer);
        if (!result)
        {
            return Result<void>{ .m_err = ErrorCode::db_open_failure };
        }

        m_db = std::move(sql3_ptr(buffer));

        return Result<void>();
    }

    Result<db_err> Database::create_table(const std::string& table_name, 
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
        auto result = sqlite3_exec(m_db.get(), sql_table.c_str(), nullptr, nullptr, &err_msg);

        auto err_msg_str = std::string(err_msg);

        sqlite3_free(err_msg);

        if(!result)
        {
            return Result<std::string>{ err_msg_str, ErrorCode::db_err };
        }

        return Result<std::string>();
    }

    Result<db_err> Database::insert_data(const std::string &table_name, 
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
        auto result = sqlite3_exec(m_db.get(), sql_insert.c_str(), nullptr, nullptr, &err_msg);
        auto err_msg_str = std::string(err_msg);
        sqlite3_free(err_msg);

        if(!result)
        {
            return Result<std::string>{ err_msg_str, ErrorCode::db_err };
        }
        return Result<std::string>();
    }

    Result<db_err> Database::select_data(const std::string &table_name, 
                                              const std::pair<std::string, std::string>& where)
    {
        std::string sql_select
        = "SELECT * FROM " + table_name 
        + "WHERE " + where.first + " = " + where.second + ";";

        char* err_msg = nullptr;
        auto result = sqlite3_exec(m_db.get(), sql_select.c_str(), nullptr, nullptr, &err_msg);
        auto err_msg_str = std::string(err_msg);
        sqlite3_free(err_msg);

        if(!result)
        {
            return Result<std::string>{ err_msg_str, ErrorCode::db_err };
        }

        return Result<std::string>();
    }

    Result<db_err> Database::update_data(const std::string &table_name, 
                                              const std::pair<std::string, std::string>& set, 
                                              const std::pair<std::string, std::string>& where)
    {
        std::string sql_update
        = "UPDATE " + table_name + " SET " + set.first + " = " + set.second 
        + " WHERE " + where.first + " = " + "'" + where.second + "'" + ";";
        
        char* err_msg = nullptr;
        auto result = sqlite3_exec(m_db.get(), sql_update.c_str(), nullptr, nullptr, &err_msg);
        auto err_msg_str = std::string(err_msg);
        sqlite3_free(err_msg);

        if(!result)
        {
            return Result<std::string>{ err_msg_str, ErrorCode::db_err };
        }
        return Result<std::string>();
    }
}