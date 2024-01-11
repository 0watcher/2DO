#include "2DOCore/user.hpp"

#include <stdexcept>

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

[[nodiscard]] tdl::Result<User, UsrDbErr> UserDb::get_user(
    const String& username) {
    auto data =
        m_db.select_data(USERS_TABLE, {"id", "username", "role", "password"},
                         {"username", username});
    if (!data) {
        return tdl::Err<User, UsrDbErr>(UsrDbErr::GetUserDataErr);
    }
    Vector<Value> usr_data = data.value();

    return tdl::Ok<User, UsrDbErr>(User{std::stoi(usr_data[0]), usr_data[1],
                                        stor(usr_data[2]), usr_data[3]});
}

[[nodiscard]] tdl::Result<User, UsrDbErr> UserDb::get_user(int id) {
    auto data = m_db.select_data(USERS_TABLE, {"username", "role", "password"},
                                 {"id", std::to_string(id)});
    if (!data) {
        return tdl::Err<User, UsrDbErr>(UsrDbErr::GetUserDataErr);
    }
    Vector<Value> usr_data = data.value();

    return tdl::Ok<User, UsrDbErr>(
        User{id, usr_data[0], stor(usr_data[1]), usr_data[2]});
}

[[nodiscard]] tdl::Result<Id, UsrDbErr> UserDb::get_user_id(
    const String& username) {
    auto id = m_db.select_data(USERS_TABLE, {"id"}, {"username", username});
    if (!id) {
        return tdl::Err<Id, UsrDbErr>(UsrDbErr::GetUserDataErr);
    }
    return tdl::Ok<Id, UsrDbErr>(stoi(id.value()[0]));
}

tdl::Result<tdl::None, UsrDbErr> UserDb::add_user(User& user) {
    auto result =
        m_db.insert_data(USERS_TABLE, {{"username", user.username()},
                                       {"role", rtos(user.role())},
                                       {"password", user.password()}});
    auto id = get_user_id(user.username());
    if (!result || !id) {
        return tdl::Err<tdl::None, UsrDbErr>(UsrDbErr::AddUserErr);
    }

    user.set_id(id.value());
    return tdl::Ok<tdl::None, UsrDbErr>({});
}

tdl::Result<tdl::None, UsrDbErr> UserDb::delete_user(const String& username) {
    auto result = m_db.delete_data(USERS_TABLE, {"username", username});
    if (!result) {
        return tdl::Err<tdl::None, UsrDbErr>(UsrDbErr::DeleteUserErr);
    }
    return tdl::Ok<tdl::None, UsrDbErr>({});
}

tdl::Result<tdl::None, UsrDbErr> UserDb::delete_user(int id) {
    auto result = m_db.delete_data(USERS_TABLE, {"id", std::to_string(id)});
    if (!result) {
        return tdl::Err<tdl::None, UsrDbErr>(UsrDbErr::DeleteUserErr);
    }
    return tdl::Ok<tdl::None, UsrDbErr>({});
}

tdl::Result<tdl::None, UsrDbErr> UserDb::update_data(const User& user) {
    auto data = m_db.select_data(USERS_TABLE, {"username", "role", "password"},
                                 {"id", std::to_string(user.id())});
    if (!data) {
        return tdl::Err<tdl::None, UsrDbErr>(UsrDbErr::GetUserDataErr);
    }

    String db_username = data.value()[0];
    String db_role = data.value()[1];
    String db_password = data.value()[2];

    String id = std::to_string(user.id());
    String username = user.username();
    String role = rtos(user.role());
    String password = user.password();

    if (username != db_username) {
        auto result =
            m_db.update_data(USERS_TABLE, {"username", username}, {"id", id});
        if (!result) {
            return tdl::Err<tdl::None, UsrDbErr>(UsrDbErr::UpdateDataErr);
        }
    }

    if (role != db_role) {
        auto result = m_db.update_data(USERS_TABLE, {"role", role}, {"id", id});
        if (!result) {
            return tdl::Err<tdl::None, UsrDbErr>(UsrDbErr::UpdateDataErr);
        }
    }

    if (password != db_password) {
        auto result =
            m_db.update_data(USERS_TABLE, {"password", password}, {"id", id});
        if (!result) {
            return tdl::Err<tdl::None, UsrDbErr>(UsrDbErr::UpdateDataErr);
        }
    }

    return tdl::Ok<tdl::None, UsrDbErr>({});
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