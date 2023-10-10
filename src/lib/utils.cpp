#include "utils.hpp"

#include <filesystem>

namespace twodo
{
[[nodiscard]] String input()
{
    String input {};
    std::getline(std::cin, input);
    if (std::cin.fail())
    {
        throw;
    }
    return input;
}

[[nodiscard]] String hash(const String& str)
{
    std::hash<String> hasher {};
    auto hashed_value = std::to_string(hasher(str));
    if (hashed_value.empty())
    {
        throw;
    }
    return hashed_value;
}

Result<None, DbError> Database::create_table(const String& table_name,
                                             const ColumnDef& column_names)
{
    String query = "CREATE TABLE IF NOT EXISTS " + table_name + " (";

    for (const auto& column : column_names)
    {
        query += column.first + " " + column.second + ", ";
    }

    if (!column_names.empty())
    {
        query.pop_back();  // Remove the last comma
        query.pop_back();  // Remove the extra space
    }

    query += ")";

    try
    {
        auto result = m_db.exec(query);
    }
    catch (const std::exception& e)
    {
        return Err<None, DbError>(DbError{e.what()});
    }

    return Ok<None, DbError>({});
}

Result<None, DbError> Database::drop_table(const String& table_name)
{
    String drop = "DROP TABLE " + table_name;

    try
    {
        auto result = m_db.exec(drop);
    }
    catch (const std::exception& e)
    {
        return Err<None, DbError>(DbError{e.what()});
    }

    return Ok<None, DbError>({});
}

Result<None, DbError> Database::insert_data(const String& table_name, const ColumnInsert& values)
{
    String query = "INSERT INTO " + table_name + " (";

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

    try
    {
        auto result = m_db.exec(query);
    }
    catch (const std::exception& e)
    {
        return Err<None, DbError>(DbError{e.what()});
    }

    return Ok<None, DbError>({});
}

Result<None, DbError> Database::delete_data(const String& table_name, const Condition& where)
{
    String query =
        "DELETE FROM " + table_name + " WHERE " + where.first + " = " + "'" + where.second + "';";

    try
    {
        auto result = m_db.exec(query);
    }
    catch (const std::exception& e)
    {
        return Err<None, DbError>(DbError{e.what()});
    }
    return Ok<None, DbError>({});
}

Result<None, DbError> Database::update_data(const String& table_name, const ColumnUpdate& set,
                                            const Condition& where)
{
    String query = "UPDATE " + table_name + " SET " + set.first + " = '" + set.second + "' WHERE " +
                   where.first + " = " + where.second + ";";

    try
    {
        auto result = m_db.exec(query);
    }
    catch (const std::exception& e)
    {
        return Err<None, DbError>(DbError{e.what()});
    }

    return Ok<None, DbError>({});
}

[[nodiscard]] Result<ColumnValues, DbError> Database::select_data(const String& table_name,
                                                                  const ColumnNames& what,
                                                                  const Condition& where)
{
    String query = "SELECT ";
    for (const auto& name : what)
    {
        query += name + ", ";
    }
    query.pop_back();  // Remove the last comma
    query.pop_back();  // Remove the extra space

    query += " FROM " + table_name;

    if (where.first.empty() || where.second.empty())
    {
        query += ';';
    }
    else
    {
        query += " WHERE " + where.first + " = '" + where.second + "';";
    }

    std::vector<std::string> values {};

    try
    {
        SQLite::Statement query_(m_db, query);
        if (query_.getColumnCount() != what.size())
        {
            return Err<ColumnValues, DbError>(DbError(DbErr::IncompatibleNumberOfColumns));
        }

        while (query_.executeStep())
        {
            for (std::size_t i = 0; i < what.size(); ++i)
            {
                values.push_back(query_.getColumn(i));
            }
        }

        if (values.empty())
        {
            return Err<ColumnValues, DbError>(DbError(DbErr::EmptyResult));
        }
    }
    catch (const std::exception& e)
    {
        return Err<ColumnValues, DbError>(DbError{e.what()});
    }

    return Ok<ColumnValues, DbError>(std::move(values));
}

[[nodiscard]] bool Database::is_table_empty(const String& table_name)
{
    try
    {
        SQLite::Statement query(m_db, "SELECT COUNT(*) FROM " + table_name);

        if (query.executeStep())
        {
            int rowCount = query.getColumn(0).getInt();
            return rowCount == 0;
        }
    }
    catch (const std::exception&)
    {
        return false;
    }
}
}  // namespace twodo