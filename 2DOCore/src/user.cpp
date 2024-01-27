#include "2DOCore/user.hpp"

#include <stdexcept>
#include "SQLiteCpp/Database.h"
#include "Utils/database.hpp"

namespace SQL = SQLite;

namespace twodocore {
[[nodiscard]] String User::rtos(Role role) const {
    switch (role) {
        case Role::Admin:
            return "Admin";
            break;
        case Role::User:
            return "User";
            break;
        default:
            throw std::logic_error("Invalid role enum!");
            break;
    }
}

[[nodiscard]] Role User::stor(const String& role_str) const {
    static const std::map<std::string, Role> role_map = {
        {"User", Role::User}, {"Admin", Role::Admin}};

    const auto it = role_map.find(role_str);
    if (it != role_map.end()) {
        return it->second;
    } else {
        throw std::logic_error("Invalid role string!");
    }
}

UserDb::UserDb(StringView db_filepath) : m_db{db_filepath, SQL::OPEN_READWRITE | SQL::OPEN_CREATE} {
    if (!is_table_empty()) {
        SQL::Statement query{
            m_db,
            "CREATE TABLE IF NOT EXISTS users ("
            "user_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
            "username VARCHAR(20) NOT NULL, "
            "role BOOLEAN NOT NULL, "
            "password VARCHAR(20) NOT NULL)"};

        query.exec();
        if (!query.isDone())
            throw std::runtime_error("Failure creating user table.");
    }
}

tdu::Result<User, DbError> UserDb::get_object(
    unsigned int id) const noexcept {
    SQL::Statement query{m_db, "SELECT * FROM users WHERE user_id = ?"};
    query.bind(1, id);

    if (!query.executeStep()) {
        return tdu::Err(DbError::SelectFailure);
    }

    const auto user = User{
        (unsigned)query.getColumn(0).getInt(), query.getColumn(1).getString(),
        query.getColumn(2).getString(), query.getColumn(3).getString()};

    return tdu::Ok(std::move(user));
}

tdu::Result<Vector<User>, DbError> UserDb::get_all_objects()
    const noexcept {
    SQL::Statement query{m_db, "SELECT * FROM users"};

    Vector<User> users;
    while (query.executeStep()) {
        users.push_back(User{(unsigned)query.getColumn(0).getInt(),
                             query.getColumn(1).getString(),
                             query.getColumn(2).getString(),
                             query.getColumn(3).getString()});
    }

    if (!query.isDone()) {
        return tdu::Err(DbError::SelectFailure);
    }

    return tdu::Ok(std::move(users));
}

bool UserDb::is_table_empty() const noexcept {
    return m_db.tableExists("users");
}

tdu::Result<void, DbError> UserDb::add_object(User& user) noexcept {
    SQL::Statement query{
        m_db, "INSERT INTO users (username, role, password) VALUES (?, ?, ?)"};
    query.bind(1, user.username());
    query.bind(2, user.role<String>());
    query.bind(3, user.password());

    if (!query.exec()) {
        return tdu::Err(DbError::InsertFailure);
    }

    query = SQL::Statement{
        m_db, "SELECT user_id FROM users ORDER BY user_id DESC LIMIT 1"};

    if (!query.executeStep()) {
        return tdu::Err(DbError::SelectFailure);
    }

    if(query.isDone()) {
        return tdu::Err(DbError::SelectFailure);
    }

    user.set_id(std::stoi(query.getColumn(0)));

    return tdu::Ok();
}

tdu::Result<void, DbError> UserDb::update_object(
    const User& user) const noexcept {
    SQL::Statement query{m_db,
                         "UPDATE users SET username = ?, role = ?, "
                         "password = ? WHERE user_id = ?"};
    query.bind(1, user.username());
    query.bind(2, user.role<String>());
    query.bind(3, user.password());
    query.bind(4, std::to_string(user.id()));

    query.exec();
    if (!query.isDone()) {
        return tdu::Err(DbError::UpdateFailure);
    }

    return tdu::Ok();
}

tdu::Result<void, DbError> UserDb::delete_object(
    unsigned int id) const noexcept {
    SQL::Statement query{m_db, "DELETE FROM users WHERE user_id = ?"};
    query.bind(1, std::to_string(id));

    query.exec();
    if (!query.isDone()) {
        return tdu::Err(DbError::DeleteFailure);
    }

    return tdu::Ok();
}
tdu::Result<User, DbError> UserDb::find_object_by_unique_column(
    const String& column_value) const noexcept {
    SQL::Statement query{m_db, "SELECT * FROM users WHERE username = ?"};
    query.bind(1, column_value);

    if (!query.executeStep()) {
        return tdu::Err(DbError::SelectFailure);
    }

    const auto user = User{
        (unsigned)query.getColumn(0).getInt(), query.getColumn(1).getString(),
        query.getColumn(2).getString(), query.getColumn(3).getString()};

    return tdu::Ok(std::move(user));
};
}  // namespace twodocore