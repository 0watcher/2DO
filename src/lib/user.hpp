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
    InvalidNameLength,
    AlreadyExistingName,
    PasswordTooShort,
    MissingUpperCase,
    MissingLowerCase,
    MissingNumber,
    MissingSpecialCharacter,       
};

enum class UsrDbErr
{
    GetUserErr,
    AddUserErr,
    DeleteUserErr,
    UpdateDataErr
};

enum class Role
{
    User,
    Admin
};

class User
{
   public:
    User(int id, const String& username, Role role, const String& password)
        : m_id {id}, m_username {username}, m_role {role}, m_password{password}
    {
    }

    int get_id() const { return m_id; }
    String get_username() const { return m_username; }
    Role get_role() const { return m_role; }
    String get_password() const { return m_password; }

    void set_id(int id) { m_id = id; }
    void set_username(const String& username) { m_username = username; }
    void set_role(Role role) { m_role = role; }
    void set_password(const String& passwd) { m_password = passwd; }

   private:
    int m_id {};
    String m_username {};
    Role m_role {};
    String m_password {};
};

class AuthManager
{
   public:
    AuthManager() = default;

    Result<User, AuthErr> login();
    Result<User, AuthErr> singup();

   private:
    Database m_db{"user"};

    Result<None, AuthErr> username_validation(const String& username) const;
    Result<None, AuthErr> password_validation(const String& password) const;
};

class UserDb
{
   public:
    UserDb();

    Result<User, UsrDbErr> get_user(const String& username) noexcept;
    Result<None, UsrDbErr> add_user(const User& user);
    Result<None, UsrDbErr> delete_user(const String& username);
    Result<None, UsrDbErr> update_data(const User& user);

   private:
    Database m_db{"user"};
};


Role stor(const String& role_str);
String rtos(Role role);

}  // namespace twodo