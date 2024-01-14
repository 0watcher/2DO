#pragma once

#include <optional>

#include <Utils/database.hpp>
#include <Utils/result.hpp>
#include <Utils/type.hpp>
#include <Utils/util.hpp>

namespace tdl = twodoutils;

namespace twodocore {
enum class UsrDbErr {
    GetUserDataErr = 1,
    AddUserErr,
    DeleteUserErr,
    UpdateDataErr
};

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

class [[nodiscard]] UserDb {
  public:
    UserDb(const UserDb&) = delete;
    UserDb& operator=(const UserDb&) = delete;
    UserDb(UserDb&& other) = default;
    UserDb& operator=(UserDb&& other) = default;

    UserDb(const String& path);

    [[nodiscard]] tdl::Result<User, UsrDbErr> get_user(const String& username) const;
    [[nodiscard]] tdl::Result<User, UsrDbErr> get_user(int id) const;
    [[nodiscard]] tdl::Result<tdl::Id, UsrDbErr> get_user_id(const String& username) const;
    [[nodiscard]] bool is_empty() const;
    
    tdl::Result<tdl::None, UsrDbErr> add_user(User& user);
    tdl::Result<tdl::None, UsrDbErr> delete_user(const String& username);
    tdl::Result<tdl::None, UsrDbErr> delete_user(int id);
    tdl::Result<tdl::None, UsrDbErr> update_data(const User& user);

  private:
    tdl::Database m_db;
};
}  // namespace twodocore