#include "Utils/database.hpp"

namespace twodoutils {
Result<void, DbError> Database::create_table(
    const String& table_name,
    const HashMap<Attribute, AttributeType>& column_def) {
    String query = "CREATE TABLE IF NOT EXISTS " + table_name + " (";

    for (const auto& column : column_def) {
        query += column.first + " " + column.second + ", ";
    }

    if (!column_def.empty()) {
        query.pop_back();  // Remove the last comma
        query.pop_back();  // Remove the extra space
    }

    query += ")";

    try {
        const auto result = m_db.exec(query);
    } catch (const std::exception& e) {
        return Err(DbError{e.what()});
    }

    return Ok();
}

Result<void, DbError> Database::drop_table(const String& table_name) {
    String drop = "DROP TABLE " + table_name;

    try {
        const auto result = m_db.exec(drop);
    } catch (const std::exception& e) {
        return Err(DbError{e.what()});
    }

    return Ok();
}

Result<void, DbError> Database::insert_data(
    const String& table_name,
    const HashMap<Attribute, Value>& values) {
    String query = "INSERT INTO " + table_name + " (";

    for (const auto& column : values) {
        query += column.first + ",";
    }
    query.pop_back();
    query += ") VALUES ";

    query += "(";
    for (const auto& value : values) {
        query += "'" + value.second + "',";
    }
    query.pop_back();
    query += ");";

    try {
        const auto result = m_db.exec(query);
    } catch (const std::exception& e) {
        return Err(DbError{e.what()});
    }

    return Ok();
}

Result<void, DbError> Database::delete_data(const String& table_name,
                                            const Condition& where) {
    String query = "DELETE FROM " + table_name + " WHERE " + where.first +
                   " = " + "'" + where.second + "';";

    try {
        const auto result = m_db.exec(query);
    } catch (const std::exception& e) {
        return Err(DbError{e.what()});
    }
    return Ok();
}

Result<void, DbError> Database::update_data(
    const String& table_name,
    const std::pair<Attribute, UpdatedValue>& set,
    const Condition& where) {
    String query = "UPDATE " + table_name + " SET " + set.first + " = '" +
                   set.second + "' WHERE " + where.first + " = " +
                   where.second + ";";

    try {
        const auto result = m_db.exec(query);
    } catch (const std::exception& e) {
        return Err(DbError{e.what()});
    }

    return Ok();
}

[[nodiscard]] Result<Vector<Value>, DbError> Database::select_data(
    const String& table_name,
    const Vector<Attribute>& what,
    const Condition& where) const {
    String query = "SELECT ";
    for (const auto& name : what) {
        query += name + ", ";
    }
    query.pop_back();  // Remove the last comma
    query.pop_back();  // Remove the extra space

    query += " FROM " + table_name;

    if (where.first.empty() || where.second.empty()) {
        query += ';';
    } else {
        query += " WHERE " + where.first + " = '" + where.second + "';";
    }

    Vector<String> values{};

    try {
        SQLite::Statement query_(m_db, query);
        if (query_.getColumnCount() != what.size()) {
            return Err(DbError(DbErr::IncompatibleNumberOfColumns));
        }

        while (query_.executeStep()) {
            for (size_t i = 0; i < what.size(); ++i) {
                values.push_back(query_.getColumn(i));
            }
        }

        if (values.empty()) {
            return Err(DbError(DbErr::EmptyResult));
        }
    } catch (const std::exception& e) {
        return Err(DbError{e.what()});
    }

    return Ok(std::move(values));
}

[[nodiscard]] bool Database::is_table_empty(const String& table_name) const {
    try {
        SQLite::Statement query(m_db, "SELECT COUNT(*) FROM " + table_name);

        if (query.executeStep()) {
            query.getColumn(0).getInt();
        }
    } catch (const std::exception&) {
        return true;
    }
    return false;
}
}  // namespace twodoutils