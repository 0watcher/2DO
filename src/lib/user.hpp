#pragma once

#include <optional>
#include <regex>
#include <string>
#include <vector>

#include "result.hpp"
#include "utils.hpp"

namespace twodo
{
enum class UserErr
{

};

enum class AuthErr
{
    InvalidNameLength = 1,
    AlreadyExistingName,
    PasswordTooShort,
    MissingUpperCase,
    MissingLowerCase,
    MissingNumber,
    MissingSpecialCharacter,
    UserNotFound,
    AllTriesExhausted,
    DbErr,
};

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
    User(int id, const String& username, Role role, const String& password)
        : m_id {id}, m_username {username}, m_role {role}, m_password {password}
    {
    }

    User(const String& username, Role role, const String& password)
        : m_username {username}, m_role {role}, m_password {password}
    {
    }

    bool operator==(const User& other) const
    {
        return m_id == other.m_id && m_username == other.m_username && m_role == other.m_role &&
               m_password == other.m_password;
    }

    [[nodiscard]] int get_id() const { return m_id.value(); }
    [[nodiscard]] String get_username() const { return m_username; }
    [[nodiscard]] Role get_role() const { return m_role; }
    [[nodiscard]] String get_password() const { return m_password; }

    void set_id(int id) { m_id = id; }
    void set_username(const String& username) { m_username = username; }
    void set_role(Role role) { m_role = role; }
    void set_password(const String& passwd) { m_password = passwd; }

   private:
    std::optional<int> m_id {std::nullopt};
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
    [[nodiscard]] Result<int, UsrDbErr> get_user_id(const String& username);
    [[nodiscard]] bool is_empty();
    Result<None, UsrDbErr> add_user(const User& user);
    Result<None, UsrDbErr> delete_user(const String& username);
    Result<None, UsrDbErr> delete_user(int id);
    Result<None, UsrDbErr> update_data(const User& user);

   private:
    Database m_db;
};

class RegisterManager
{
   public:
    RegisterManager(UserDb&& udb, IUserInputHandler& ihandler, IDisplayer& idisplayer)
        : m_udb {std::move(udb)}, m_ihandler {ihandler}, m_idisplayer {idisplayer}
    {
    }

    [[nodiscard]] Result<User, AuthErr> singup();
    [[nodiscard]] Result<None, AuthErr> username_validation(const String& username) const;
    [[nodiscard]] Result<None, AuthErr> password_validation(const String& password) const;

   private:
    UserDb m_udb;
    IUserInputHandler& m_ihandler;
    IDisplayer& m_idisplayer;
};

class AuthManager
{
   public:
    AuthManager(UserDb&& udb, IUserInputHandler& ihandler, IDisplayer& idisplayer)
        : m_udb {std::move(udb)}, m_ihandler {ihandler}, m_idisplayer {idisplayer}
    {
    }

    [[nodiscard]] Result<User, AuthErr> login();
    [[nodiscard]] Result<None, AuthErr> auth_username();
    [[nodiscard]] Result<User, AuthErr> auth_password(const String& username);

   private:
    UserDb m_udb;
    IUserInputHandler& m_ihandler;
    IDisplayer& m_idisplayer;
};
}  // namespace twodo