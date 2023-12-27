#pragma once

#include <optional>
#include <regex>
#include <string>
#include <vector>
#include <string_view>

#include "result.hpp"
#include "utils.hpp"

namespace twodocore
{
enum class UsrDbErr
{
    GetUserDataErr,
    AddUserErr,
    DeleteUserErr,
    UpdateDataErr
};

enum class Role
{
    User,
    Admin
};

[[nodiscard]] Role stor(const String& role_str);
[[nodiscard]] String rtos(Role role);

class User
{
   public:
    User(int user_id, std::string_view username, Role role, std::string_view password)
        : m_user_id {user_id}, m_username {username}, m_role {role}, m_password {password}
    {
    }

    User(std::string_view username, Role role, std::string_view password)
        : m_username {username}, m_role {role}, m_password {password}
    {
    }

    bool operator==(const User& other) const
    {
        return m_user_id == other.m_user_id && m_username == other.m_username && m_role == other.m_role &&
               m_password == other.m_password;
    }

    [[nodiscard]] int get_id() const { return m_user_id.value(); }
    [[nodiscard]] String get_username() const { return m_username; }
    [[nodiscard]] Role get_role() const { return m_role; }
    [[nodiscard]] String get_password() const { return m_password; }

    void set_id(int user_id) { m_user_id = user_id; }
    void set_username(std::string_view username) { m_username = username; }
    void set_role(Role role) { m_role = role; }
    void set_password(std::string_view passwd) { m_password = passwd; }

   private:
    std::optional<int> m_user_id {std::nullopt};
    String m_username {};
    Role m_role {};
    String m_password {};
};

class UserDb
{
   public:
    UserDb(const String& path);

    UserDb(const UserDb&) = delete;
    UserDb& operator=(const UserDb&) = delete;
    UserDb(UserDb&& other) = default;
    UserDb& operator=(UserDb&& other) = default;

    [[nodiscard]] Result<User, UsrDbErr> get_user(const String& username);
    [[nodiscard]] Result<User, UsrDbErr> get_user(int id);
    [[nodiscard]] Result<Id, UsrDbErr> get_user_id(const String& username);
    [[nodiscard]] bool is_empty();
    Result<None, UsrDbErr> add_user(User& user);
    Result<None, UsrDbErr> delete_user(const String& username);
    Result<None, UsrDbErr> delete_user(int id);
    Result<None, UsrDbErr> update_data(const User& user);

   private:
    Database m_db;
};
}  // namespace twodo