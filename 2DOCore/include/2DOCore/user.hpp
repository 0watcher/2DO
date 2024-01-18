#pragma once

#include <optional>

#include <Utils/database.hpp>
#include <Utils/result.hpp>
#include <Utils/type.hpp>
#include <Utils/util.hpp>
#include <string>
#include "SQLiteCpp/Statement.h"

namespace tdl = twodoutils;

namespace twodocore {
// enum class UsrDbErr {
//     GetUserDataErr = 1,
//     AddUserErr,
//     DeleteUserErr,
//     UpdateDataErr
// };
enum class [[nodiscard]] Role{User, Admin};

[[nodiscard]] Role stor(const String& role_str);
[[nodiscard]] String rtos(Role role);

class [[nodiscard]] User {
  public:
    User(const User&) = default;
    User& operator=(const User&) = default;
    User(User&& other) = default;
    User& operator=(User&& other) = default;

    User(int user_id, StringView username, Role role, StringView password)
        : m_user_id{user_id},
          m_username{username},
          m_role{role},
          m_password{password} {}

    User(StringView username, Role role, StringView password)
        : m_username{username}, m_role{role}, m_password{password} {}

    bool operator==(const User& other) const {
        return m_user_id == other.m_user_id && m_username == other.m_username &&
               m_role == other.m_role && m_password == other.m_password;
    }

    [[nodiscard]] int id() const { return m_user_id.value(); }
    [[nodiscard]] String username() const { return m_username; }
    [[nodiscard]] Role role() const { return m_role; }
    [[nodiscard]] String password() const { return m_password; }

    void set_id(int user_id) { m_user_id = user_id; }
    void set_username(StringView username) { m_username = username; }
    void set_role(Role role) { m_role = role; }
    void set_password(StringView passwd) { m_password = passwd; }

  private:
    std::optional<int> m_user_id{std::nullopt};
    String m_username{};
    Role m_role{};
    String m_password{};
};

class [[nodiscard]] UserDb : protected tdl::Database<User> {
    UserDb(const UserDb&) = delete;
    UserDb& operator=(const UserDb&) = delete;
    UserDb(UserDb&& other) = default;
    UserDb& operator=(UserDb&& other) = default;

    UserDb(StringView db_filepath) {
        m_db = SQLite::Database{db_filepath};

        if (!m_db.tableExists("users")) {
            SQLite::Statement query{
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

    tdl::Result<User, tdl::DbError> get_object_by_column_name(
        const String& column_name,
        const String& column_value) const noexcept override {
        SQLite::Statement query{m_db, "SELECT * FROM users WHERE ? = ?"};
        query.bind(1, column_name);
        query.bind(2, column_value);

        query.exec();
        const auto user =
            User{query.getColumn(0).getInt(), query.getColumn(1).getString(),
                 stor(query.getColumn(2).getString()),
                 query.getColumn(3).getString()};

        if (!query.isDone()) {
            return tdl::Err(tdl::DbError::SelectFailure);
        }

        return tdl::Ok(std::move(user));
    }

    tdl::Result<User, tdl::DbError> get_object_by_id(
        int id) const noexcept override {
        const auto user =
            get_object_by_column_name("user_id", std::to_string(id));

        if (!user) {
            return tdl::Err(tdl::DbError::SelectFailure);
        }

        return tdl::Ok(std::move(user.unwrap()));
    }

    tdl::Result<void, tdl::DbError> add_object(User& obj) noexcept override {
        SQLite::Statement query{
            m_db,
            "INSERT INTO users (username, role, password) VALUES (?, ?, ?)"};
        query.bind(1, obj.username());
        query.bind(2, rtos(obj.role()));
        query.bind(3, obj.username());

        query.exec();
        if (!query.isDone()) {
            return tdl::Err(tdl::DbError::InsertFailure);
        }

        query = SQLite::Statement{
            m_db, "SELECT id FROM users ORDER BY id DESC LIMIT 1"};
        query.exec();

        if (!query.isDone()) {
            return tdl::Err(tdl::DbError::SelectFailure);
        }

        obj.set_id(std::stoi(query.getColumn(0)));

        return tdl::Ok();
    }

    tdl::Result<void, tdl::DbError> update_object(
        const User& obj) const noexcept override {
        SQLite::Statement query{m_db,
                                "UPDATE users SET username = ?, role = ?, "
                                "password = ? WHERE id = ?"};
        query.bind(1, obj.username());
        query.bind(2, rtos(obj.role()));
        query.bind(3, obj.password());
        query.bind(4, std::to_string(obj.id()));

        query.exec();
        if (!query.isDone()) {
            return tdl::Err(tdl::DbError::UpdateFailure);
        }

        return tdl::Ok();
    }

    tdl::Result<void, tdl::DbError> delete_object(
        const User& obj) const noexcept override {
        SQLite::Statement query{m_db, "DELETE FROM users WHERE id = ?"};
        query.bind(2, std::to_string(obj.id()));

        query.exec();
        if (!query.isDone()) {
            return tdl::Err(tdl::DbError::DeleteFailure);
        }

        return tdl::Ok();
    }

    tdl::Result<Vector<User>, tdl::DbError> get_all_objects(
        const String& table_name) const noexcept override {
        SQLite::Statement query{m_db, "SELECT * FROM ?"};
        query.bind(1, table_name);

        Vector<User> users;
        while (query.executeStep()) {
            users.push_back(User{query.getColumn(0).getInt(),
                                 query.getColumn(1).getString(),
                                 stor(query.getColumn(2).getString()),
                                 query.getColumn(3).getString()});
        }

        if(!query.isDone()) {
          return tdl::Err(tdl::DbError::SelectFailure);
        }

        return tdl::Ok(std::move(users));
    }
};
}  // namespace twodocore