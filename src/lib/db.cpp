#include "db.hpp"

namespace twodo
{
Result<None, DbError> Db::create_table(const String& table_name, const stringmap& column_names) 
{
    std::string query = "CREATE TABLE IF NOT EXISTS " + table_name + " (";

    for (const auto& column : column_names)
    {
        query += column.first + " " + column.second + ", ";
    }

    if (!column_names.empty())
    {
        query.pop_back(); // Remove the last comma
        query.pop_back(); // Remove the extra space
    }

    query += ")";
    
    try
    {
        auto result = m_db.exec(query);
    }
    catch(const std::exception& e)
    {
        return Err<None, DbError>(String(e.what()));
    }

    return Ok<None, DbError>({});
}

Result<None, DbError> Db::drop_table(const String& table_name) 
{
    std::string drop = "DROP TABLE " + table_name;

    try
    {
        auto result = m_db.exec(drop);
    }
    catch(const std::exception& e)
    {
        return Err<None, DbError>(String(e.what()));
    }

    return Ok<None, DbError>({});
}

Result<None, DbError> Db::insert_data(const String& table_name, const stringmap& values) 
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


    try
    {
        auto result = m_db.exec(query);
    }
    catch(const std::exception& e)
    {
        return Err<None, DbError>(String(e.what()));
    }

    return Ok<None, DbError>({});
}

Result<None, DbError> Db::update_data(const String& table_name, const stringpair& set,
                                      const stringpair& where)
{
    std::string query = "UPDATE " + table_name + " SET " + set.first + " = '" + set.second +
                        "' WHERE " + where.first + " = " + where.second + ";";

    try
    {
        auto result = m_db.exec(query);
    }
    catch(const std::exception& e)
    {
        return Err<None, DbError>(String(e.what()));
    }

    return Ok<None, DbError>({});
}

Result<stringvec, DbError> Db::select_data(const String& table_name, const stringvec& what,
                                           const stringpair& where)
{
    std::string query = "SELECT ";
    for (const auto& name : what)
    {
        query += name + ", ";
    }
    query.pop_back(); // Remove the last comma
    query.pop_back(); // Remove the extra space

    query += " FROM " + table_name;
    
    if(where.first.empty() || where.second.empty())
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
            return Err<stringvec, DbError>(DbErr::IncompatibleNumberOfColumns);
        }


        while (query_.executeStep())
        {
            for (std::size_t i = 0; i < what.size(); ++i)
            {
                values.push_back(query_.getColumn(i));
            }
        }
    }
    catch(const std::exception& e)
    {
        return Err<stringvec, DbError>(String(e.what()));
    }
    
    return Ok<stringvec, DbError>(values);
}
}  // namespace twodo
