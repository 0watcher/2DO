#pragma once

#include <optional>
#include <regex>
#include <string>
#include <vector>

#include "database.hpp"
#include "result.hpp"
#include "utils.hpp"

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
    UserNotFound,
    AllTriesExhausted
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

Role stor(const String& role_str);
String rtos(Role role);

class User
{
   public:
    User(int id, const String& username, Role role, const String& password)
        : m_id {id}, m_username {username}, m_role {role}, m_password {password} {}

    User(const String& username, Role role, const String& password)
        : m_username {username}, m_role {role}, m_password {password} {}


    int get_id() const { return m_id.value(); }
    String get_username() const { return m_username; }
    Role get_role() const { return m_role; }
    String get_password() const { return m_password; }

    void set_id(int id) { m_id = id; }
    void set_username(const String& username) { m_username = username; }
    void set_role(Role role) { m_role = role; }
    void set_password(const String& passwd) { m_password = passwd; }

   private:
    std::optional<int> m_id {};
    String m_username {};
    Role m_role {};
    String m_password {};
};

class UserDb
{
   public:
    UserDb();
    Result<User, UsrDbErr> get_user(const String& username) noexcept;
    Result<None, UsrDbErr> add_user(const User& user);
    Result<None, UsrDbErr> delete_user(const String& username);
    Result<None, UsrDbErr> update_data(const User& user);
    bool is_empty();

   private:
    Database m_db {"user"};
};

class RegisterManager
{
   public:
    RegisterManager(IUserInputHandler& ihandler) : m_ihandler {ihandler} {}
    Result<User, AuthErr> singup();

   private:
    UserDb m_udb {};
    IUserInputHandler& m_ihandler;

    Result<None, AuthErr> username_validation(const String& username) const;
    Result<None, AuthErr> password_validation(const String& password) const;
};

class AuthManager
{
   public:
    AuthManager() = delete;
    AuthManager(IUserInputHandler& ihandler) : m_ihandler {ihandler} {}
    Result<User, AuthErr> login();
    Result<None, AuthErr> auth_username();
    Result<User, AuthErr> auth_password(const String& username);

   private:
    UserDb m_udb {};
    IUserInputHandler& m_ihandler;
};
}  // namespace twodo