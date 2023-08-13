#include "db.hpp"

namespace twodo
{
    Result<None, DbError> DB::create_table(const std::string& table_name,
        const std::map<std::string, std::string>& column_names)
    {
        std::string query = "CREATE TABLE IF NOT EXIST " + table_name + " (";

        for (const auto& column : column_names)
        {
            query += column.first + " " + column.second + ", ";
        }

        if (!column_names.empty())
        {
            query.pop_back();
        }

        query += ");";

        auto result = m_db.exec(query);
        if (!result)
        {
            return Error<None, DbError>(DbError::TableCreateFailure);
        }

        return Ok<DbError>();
    }

    Result<None, DbError> DB::insert_data(const std::string& table_name,
        const std::map<std::string, std::string>& values)
    {
        std::string query = "INSERT INTO " + table_name + +" (";

        for (const auto& column : values)
        {
            query += column.first + ",";
        }
        query.pop_back();
        query += ") VALUES ";

        query += "(";
        for (const auto& value : values)
        {
            query += "'" + value.second + "',";
        }
        query.pop_back();
        query += ");";

        auto result = m_db.exec(query);
        if (!result)
        {
            return Error<None, DbError>(DbError::InsertFailure);
        }

        return Ok<DbError>();
    }

    Result<None, DbError> DB::update_data(const std::string& table_name,
        const std::pair<std::string, std::string>& set,
        const std::pair<std::string, std::string>& where)
    {
        std::string query
            = "UPDATE " + table_name + " SET " + set.first + " = " + set.second
            + " WHERE " + where.first + " = " + "'" + where.second + "'" + ";";

        auto result = m_db.exec(query);
        if (!result)
        {
            return Error<None, DbError>(DbError::UpdateFailure);
        }

        return Ok<DbError>();
    }

    Result<std::vector<std::string>, DbError> DB::select_data(const std::string& table_name,
        const std::pair<std::string, std::string>& where)
    {
        std::string query_
            = "SELECT * FROM " + table_name
            + "WHERE " + where.first + " = " + where.second + ";";

        try
        {
            SQLite::Statement query(m_db, query_);

            std::vector<std::string> values{};

            return Ok<std::vector<std::string>, DbError>(values);
        }
        catch (...)
        {
            return Error<std::vector<std::string>, DbError>(DbError::SelectFailure);
        }
        
    }
}
