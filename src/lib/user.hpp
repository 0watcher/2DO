#pragma once

#include <optional>
#include <regex>
#include <string>
#include <vector>
#include "database.hpp"
#include "result.hpp"
#include "task.hpp"

using String = std::string;

namespace twodo
{
enum class UserErr
{

};

enum class AuthErr
{
    IncorrectNickname,
    AlreadyExistingName,
    IncorrectPassword
};

enum class UsrDbErr
{
    CannotGetUser
};

enum class Role
{
    User,
    Admin
};

class User
{
   public:
    User(const int& id, const String nickname, const Role& role, const String& password)
        : m_id {id}, m_nickname {nickname}, m_role {role}, m_password{password}
    {
    }

    int get_id() const { return m_id; }
    String get_nickname() const { return m_nickname; }
    Role get_role() const { return m_role; }
    String get_password() { return m_password; }

    void set_id(int id) { m_id = id; }
    void set_nickname(const String& nickname) { m_nickname = nickname; }
    void set_role(Role role) { m_role = role; }
    void set_password(const String& passwd) { m_password = passwd; }

   private:
    int m_id {};
    String m_nickname {};
    Role m_role {};
    String m_password {};
};

class AuthenticationManager
{
   public:
    AuthenticationManager() = default;

    Result<String, AuthErr> auth_nickname(const String& nickname) const;
    Result<String, AuthErr> auth_password(const String& password);

   private:
    const bool is_passwd_format_valid(const String& password) const;
};

class UserDb
{
   public:
    UserDb();

    Result<User, UsrDbErr> get_user(const String& username) noexcept;
    Result<None, UsrDbErr> add_user(const User& user) const;
    Result<None, UsrDbErr> delete_user() const;
    Result<None, UsrDbErr> modify_data();

   private:
    Database m_db{"users"};

    Role stor(const String& role_str);
};

}  // namespace twodo