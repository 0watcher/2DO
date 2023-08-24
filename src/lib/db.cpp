#include "db.hpp"

#include <cstdlib>

namespace twodo
{
Result<None, DbError> Db::create_table(const String& table_name, const stringmap& column_names) noexcept
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

Result<None, DbError> Db::drop_table(const String& table_name) noexcept
{
    std::string drop = "DROP " + table_name;
    auto result = m_db.exec(drop);
    if (!result)
    {
        return Err<None, DbError>(DbError::TableDropFailure);
    }
    return Ok<None, DbError>({});
}

Result<None, DbError> Db::insert_data(const String& table_name, const stringmap& values) noexcept
{
    std::string query = "INSERT INTO " + table_name + " (";

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

Result<None, DbError> Db::update_data(const String& table_name, const stringpair& set,
                                      const stringpair& where) noexcept
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

Result<stringvec, DbError> Db::select_data(const String& table_name, const stringvec& who,
                                           const stringpair& where) noexcept
{
    std::string query = "SELECT ";
    for (auto& name : who)
    {
        query += name + ", ";
    }
    query.pop_back();

    query += " FROM " + table_name + "WHERE " + where.first + " = " + where.second + ";";

    SQLite::Statement query_(m_db, query);
    if (query_.getColumnCount() == 0)
    {
        return Err<stringvec, DbError>(DbError::SelectFailure);
    }

    std::vector<std::string> values {};

    std::uint8_t i = 0;
    while (query_.executeStep())
    {
        values.push_back(query_.getColumn(i));
        i++;
    }

    return Ok<stringvec, DbError>(values);
}
}  // namespace twodo
