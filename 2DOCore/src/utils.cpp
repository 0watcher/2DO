#include "2DOCore/utils.hpp"

#include <fstream>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>

namespace twodocore
{

std::optional<std::string> get_base_directory()
{
#ifdef _WIN32
    const auto home = fs::path(getenv("USERPROFILE"));
#else
    const auto home = fs::path(getenv("HOME"));
#endif

    if (!home.empty())
    {
        return (home / ".config").string();
    }
    else
    {
        return std::nullopt;
    }
}

void create_simple_app_env(const std::string& folder_name, const std::vector<std::string>& files)
{
    const auto base_directory = get_base_directory();

    if (!base_directory)
    {
        throw std::runtime_error("Error: Unable to determine the base directory.");
    }

    const fs::path folder_path = fs::path(*base_directory) / folder_name;

    try
    {
        fs::create_directories(folder_path);
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error("Error: Unable to create app base folder.");
    }

    for (const auto& file_name : files)
    {
        const fs::path file_path = folder_path / file_name;
        std::ofstream file {file_path};

        if (!file.is_open())
        {
            throw std::runtime_error("Error: Unable to create env app file.");
        }
    }
}

void wipe_simple_app_env(const std::string& folder_name)
{
    const auto base_directory = get_base_directory();

    if (!base_directory)
    {
        std::cerr << "Error: Unable to determine the base directory." << std::endl;
        return;
    }

    const fs::path folder_path = fs::path(*base_directory) / folder_name;

    try
    {
        fs::remove_all(folder_path);
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error("Error: Failure wiping env app data.");
    }
}

[[nodiscard]] String tptos(const TimePoint& tp) noexcept
{
    std::time_t time = std::chrono::system_clock::to_time_t(tp);

    std::chrono::minutes minutes =
        std::chrono::duration_cast<std::chrono::minutes>(tp.time_since_epoch());

    auto minutes_count = minutes.count();

    String timeString = std::to_string(minutes_count);

    return timeString;
}

[[nodiscard]] TimePoint stotp(const String& stringified_tp) noexcept
{
    auto minutes_count = std::stoll(stringified_tp);

    TimePoint tp = std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes>(
        std::chrono::minutes(minutes_count));

    return tp;
}

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
                                             const std::map<Attribute, AttributeType>& column_def)
{
    String query = "CREATE TABLE IF NOT EXISTS " + table_name + " (";

    for (const auto& column : column_def)
    {
        query += column.first + " " + column.second + ", ";
    }

    if (!column_def.empty())
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
        return Err<None, DbError>(DbError {e.what()});
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
        return Err<None, DbError>(DbError {e.what()});
    }

    return Ok<None, DbError>({});
}

Result<None, DbError> Database::insert_data(const String& table_name,
                                            const std::map<Attribute, Value>& values)
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
        return Err<None, DbError>(DbError {e.what()});
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
        return Err<None, DbError>(DbError {e.what()});
    }
    return Ok<None, DbError>({});
}

Result<None, DbError> Database::update_data(const String& table_name,
                                            const std::pair<Attribute, UpdatedValue>& set,
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
        return Err<None, DbError>(DbError {e.what()});
    }

    return Ok<None, DbError>({});
}

[[nodiscard]] Result<std::vector<Value>, DbError> Database::select_data(
    const String& table_name, const std::vector<Attribute>& what, const Condition& where)
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
            return Err<std::vector<Value>, DbError>(DbError(DbErr::IncompatibleNumberOfColumns));
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
            return Err<std::vector<Value>, DbError>(DbError(DbErr::EmptyResult));
        }
    }
    catch (const std::exception& e)
    {
        return Err<std::vector<Value>, DbError>(DbError {e.what()});
    }

    return Ok<std::vector<Value>, DbError>(std::move(values));
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
        return true;
    }
    return false;
}
}  // namespace twodocore