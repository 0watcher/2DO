#include "2DOCore/user.hpp"

#include <algorithm>
#include <exception>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "2DOCore/result.hpp"
#include "2DOCore/utils.hpp"

#define USERS_TABLE "users"

namespace twodocore {

UserDb::UserDb(const String& path) : m_db{path} {
    if (m_db.is_table_empty(USERS_TABLE)) {
        auto result = m_db.create_table(
            USERS_TABLE, {{"id", "INTEGER PRIMARY KEY AUTOINCREMENT"},
                          {"username", "TEXT"},
                          {"role", "TEXT"},
                          {"password", "TEXT"}});
        if (!result) {
            throw std::runtime_error("Failed create the table: " +
                                     result.err().sql_err());
        }
    }
}

[[nodiscard]] Result<User, UsrDbErr> UserDb::get_user(const String& username) {
    auto data =
        m_db.select_data(USERS_TABLE, {"id", "username", "role", "password"},
                         {"username", username});
    if (!data) {
        return Err<User, UsrDbErr>(UsrDbErr::GetUserDataErr);
    }
    std::vector<Value> usr_data = data.value();

    return Ok<User, UsrDbErr>(User{std::stoi(usr_data[0]), usr_data[1],
                                   stor(usr_data[2]), usr_data[3]});
}

[[nodiscard]] Result<User, UsrDbErr> UserDb::get_user(int id) {
    auto data = m_db.select_data(USERS_TABLE, {"username", "role", "password"},
                                 {"id", std::to_string(id)});
    if (!data) {
        return Err<User, UsrDbErr>(UsrDbErr::GetUserDataErr);
    }
    std::vector<Value> usr_data = data.value();

    return Ok<User, UsrDbErr>(
        User{id, usr_data[0], stor(usr_data[1]), usr_data[2]});
}

[[nodiscard]] Result<Id, UsrDbErr> UserDb::get_user_id(const String& username) {
    auto id = m_db.select_data(USERS_TABLE, {"id"}, {"username", username});
    if (!id) {
        return Err<int, UsrDbErr>(UsrDbErr::GetUserDataErr);
    }
    return Ok<int, UsrDbErr>(stoi(id.value()[0]));
}

Result<None, UsrDbErr> UserDb::add_user(User& user) {
    auto result =
        m_db.insert_data(USERS_TABLE, {{"username", user.get_username()},
                                       {"role", rtos(user.get_role())},
                                       {"password", user.get_password()}});
    auto id = get_user_id(user.get_username());
    if (!result || !id) {
        return Err<None, UsrDbErr>(UsrDbErr::AddUserErr);
    }

    user.set_id(id.value());
    return Ok<None, UsrDbErr>({});
}

Result<None, UsrDbErr> UserDb::delete_user(const String& username) {
    auto result = m_db.delete_data(USERS_TABLE, {"username", username});
    if (!result) {
        return Err<None, UsrDbErr>(UsrDbErr::DeleteUserErr);
    }
    return Ok<None, UsrDbErr>({});
}

Result<None, UsrDbErr> UserDb::delete_user(int id) {
    auto result = m_db.delete_data(USERS_TABLE, {"id", std::to_string(id)});
    if (!result) {
        return Err<None, UsrDbErr>(UsrDbErr::DeleteUserErr);
    }
    return Ok<None, UsrDbErr>({});
}

Result<None, UsrDbErr> UserDb::update_data(const User& user) {
    auto data = m_db.select_data(USERS_TABLE, {"username", "role", "password"},
                                 {"id", std::to_string(user.get_id())});
    if (!data) {
        return Err<None, UsrDbErr>(UsrDbErr::GetUserDataErr);
    }

    String db_username = data.value()[0];
    String db_role = data.value()[1];
    String db_password = data.value()[2];

    String id = std::to_string(user.get_id());
    String username = user.get_username();
    String role = rtos(user.get_role());
    String password = user.get_password();

    if (username != db_username) {
        auto result =
            m_db.update_data(USERS_TABLE, {"username", username}, {"id", id});
        if (!result) {
            return Err<None, UsrDbErr>(UsrDbErr::UpdateDataErr);
        }
    }

    if (role != db_role) {
        auto result = m_db.update_data(USERS_TABLE, {"role", role}, {"id", id});
        if (!result) {
            return Err<None, UsrDbErr>(UsrDbErr::UpdateDataErr);
        }
    }

    if (password != db_password) {
        auto result =
            m_db.update_data(USERS_TABLE, {"password", password}, {"id", id});
        if (!result) {
            return Err<None, UsrDbErr>(UsrDbErr::UpdateDataErr);
        }
    }

    return Ok<None, UsrDbErr>({});
}

[[nodiscard]] bool UserDb::is_empty() {
    return m_db.is_table_empty(USERS_TABLE);
}

[[nodiscard]] String rtos(Role role) {
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

[[nodiscard]] Role stor(const String& role_str) {
    static const std::map<std::string, Role> role_map = {
        {"User", Role::User}, {"Admin", Role::Admin}};

    auto it = role_map.find(role_str);
    if (it != role_map.end()) {
        return it->second;
    } else {
        throw std::logic_error("Invalid role string!");
    }
}
}  // namespace twodocore