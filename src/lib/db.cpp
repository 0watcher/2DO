#include "db.hpp"

namespace twodo
{
    Result<None, DbError> Db::create_table(const String& table_name, const stringmap& column_names)
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
            return Err<None, DbError>(DbError::TableCreateFailure);
        }

        return Ok<None, DbError>({});
    }

    Result<None, DbError> Db::insert_data(const String& table_name, const stringmap& values)
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
            return Err<None, DbError>(DbError::InsertFailure);
        }

        return Ok<None, DbError>({});
    }

    Result<None, DbError> Db::update_data(const String& table_name,
        const stringpair& set,
        const stringpair& where)
    {
        std::string query = "UPDATE " + table_name + " SET " + set.first + " = " + set.second +
            " WHERE " + where.first + " = " + "'" + where.second + "'" + ";";

        auto result = m_db.exec(query);
        if (!result)
        {
            return Err<None, DbError>(DbError::UpdateFailure);
        }

        return Ok<None, DbError>({});
    }

    Result<stringvec, DbError> Db::select_data(const String& table_name, const stringpair& where)
    {
        std::string query_ =
            "SELECT * FROM " + table_name + "WHERE " + where.first + " = " + where.second + ";";

        try
        {
            SQLite::Statement query(m_db, query_);

            std::vector<std::string> values{};

            return Ok<stringvec, DbError>(values);
        }
        catch (...)
        {
            return Err<stringvec, DbError>(DbError::SelectFailure);
        }
    }
}  // namespace twodo
