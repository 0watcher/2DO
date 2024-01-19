#include "2DOCore/user.hpp"

#include <stdexcept>

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

UserDb::UserDb(StringView db_filepath) {
    m_db = SQLite::Database{db_filepath};

    if (!is_table_empty()) {
        SQL::Statement query{
            m_db,
            "CREATE TABLE users IF NOT EXISTS (user_id INTEGER "
            "AUTOINCREMENT PRIMARY "
            "KEY NOT NULL, username VARCHAR(20) NOT NULL, role BOOLEAN NOT "
            "NULL, password VARCHAR(20) NOT NULL)"};

        query.exec();
        if (!query.isDone())
            throw std::runtime_error("Failure creating user table.");
    }
}

tdl::Result<User, tdl::DbError> UserDb::get_object_by_id(
    int id) const noexcept {
    SQL::Statement query{m_db, "SELECT * FROM users WHERE id = ?"};
    query.bind(1, id);

    query.exec();
    const auto user =
        User{query.getColumn(0).getInt(), query.getColumn(1).getString(),
             query.getColumn(2).getString(), query.getColumn(3).getString()};

    if (!query.isDone()) {
        return tdl::Err(tdl::DbError::SelectFailure);
    }

    return tdl::Ok(std::move(user));
}

tdl::Result<Vector<User>, tdl::DbError> UserDb::get_all_objects()
    const noexcept {
    SQL::Statement query{m_db, "SELECT * FROM users"};

    Vector<User> users;
    while (query.executeStep()) {
        users.push_back(User{
            query.getColumn(0).getInt(), query.getColumn(1).getString(),
            query.getColumn(2).getString(), query.getColumn(3).getString()});
    }

    if (!query.isDone()) {
        return tdl::Err(tdl::DbError::SelectFailure);
    }

    return tdl::Ok(std::move(users));
}

bool UserDb::is_table_empty() const noexcept {
    return m_db.tableExists("users");
}

tdl::Result<void, tdl::DbError> UserDb::add_object(User& user) noexcept {
    SQL::Statement query{
        m_db, "INSERT INTO users (username, role, password) VALUES (?, ?, ?)"};
    query.bind(1, user.username());
    query.bind(2, user.role<String>());
    query.bind(3, user.username());

    query.exec();
    if (!query.isDone()) {
        return tdl::Err(tdl::DbError::InsertFailure);
    }

    query = SQL::Statement{
        m_db, "SELECT user_id FROM users ORDER BY user_id DESC LIMIT 1"};
    query.exec();

    if (!query.isDone()) {
        return tdl::Err(tdl::DbError::SelectFailure);
    }

    user.set_id(std::stoi(query.getColumn(0)));

    return tdl::Ok();
}

tdl::Result<void, tdl::DbError> UserDb::update_object(
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
        return tdl::Err(tdl::DbError::UpdateFailure);
    }

    return tdl::Ok();
}

tdl::Result<void, tdl::DbError> UserDb::delete_object(
    const User& user) const noexcept {
    SQL::Statement query{m_db, "DELETE FROM users WHERE user_id = ?"};
    query.bind(2, std::to_string(user.id()));

    query.exec();
    if (!query.isDone()) {
        return tdl::Err(tdl::DbError::DeleteFailure);
    }

    return tdl::Ok();
}
}  // namespace twodocore