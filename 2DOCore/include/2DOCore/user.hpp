#pragma once

#include <optional>

#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <Utils/database.hpp>
#include <Utils/result.hpp>
#include <Utils/type.hpp>
#include <Utils/util.hpp>

namespace tdu = twodoutils;
namespace SQL = SQLite;

namespace twodocore {
enum class [[nodiscard]] Role{User, Admin};

class [[nodiscard]] User {
  public:
    User(const User&) = default;
    User& operator=(const User&) = default;
    User(User&& other) = default;
    User& operator=(User&& other) = default;

    User(unsigned int user_id,
         StringView username,
         Role role,
         const String& password)
        : m_user_id{user_id},
          m_username{username},
          m_role{role},
          m_password{tdu::hash(password)} {}

    User(unsigned int user_id,
         StringView username,
         const String& role,
         const String& password)
        : m_user_id{user_id},
          m_username{username},
          m_role{stor(role)},
          m_password{password} {}

    User(StringView username, Role role, const String& password)
        : m_username{username}, m_role{role}, m_password{tdu::hash(password)} {}

    bool operator==(const User& other) const {
        return m_user_id == other.m_user_id && m_username == other.m_username &&
               m_role == other.m_role && m_password == other.m_password;
    }

    [[nodiscard]] unsigned int id() const { return m_user_id; }
    [[nodiscard]] String username() const { return m_username; }
    template <typename T>
    [[nodiscard]] typename std::enable_if<std::is_same<T, String>::value ||
                                              std::is_same<T, Role>::value,
                                          T>::type
    role() const {
        if constexpr (std::is_same<T, String>::value) {
            return rtos(m_role);
        } else if constexpr (std::is_same<T, Role>::value) {
            return m_role;
        }
    }

    [[nodiscard]] String password() const { return m_password; }

    void set_id(unsigned int user_id) { m_user_id = user_id; }
    void set_username(StringView username) { m_username = username; }
    void set_role(Role role) { m_role = role; }
    void set_password(const String& passwd) { m_password = tdu::hash(passwd); }

  private:
    unsigned int m_user_id;
    String m_username{};
    Role m_role{};
    String m_password{};

    [[nodiscard]] Role stor(const String& role_str) const;
    [[nodiscard]] String rtos(Role role) const;
};

class [[nodiscard]] UserDb {
  public:
    UserDb(const UserDb&) = delete;
    UserDb& operator=(const UserDb&) = delete;
    UserDb(UserDb&& other) = default;
    UserDb& operator=(UserDb&& other) = default;

    UserDb(StringView db_filepath);

    User get_object(unsigned int id) const noexcept;

    std::optional<User> find_object_by_unique_column(
        const String& column_value) const noexcept;

    Vector<User> get_all_objects() const noexcept;

    bool is_table_empty() const noexcept;

    void add_object(User& user) noexcept;

    void update_object(const User& user) const noexcept;

    void delete_object(unsigned int id) const noexcept;

  private:
    SQL::Database m_db;
};

enum class AuthErr {
    InvalidNameLength = 1,
    AlreadyExistingName,
    InvalidPassLength,
    MissingUpperCase,
    MissingLowerCase,
    MissingNumber,
    MissingSpecialCharacter,
    UserNotFound,
    AllTriesExhausted,
    DbErr,
};

class [[nodiscard]] AuthenticationManager {
  public:
    AuthenticationManager(AuthenticationManager&& other) = default;
    AuthenticationManager& operator=(AuthenticationManager&& other) = default;
    AuthenticationManager(const AuthenticationManager&) = delete;
    AuthenticationManager& operator=(const AuthenticationManager&) = delete;

    AuthenticationManager(std::shared_ptr<UserDb> user_db)
        : m_user_db{user_db} {}

    tdu::Result<void, AuthErr> username_validation(const String& username);
    tdu::Result<void, AuthErr> password_validation(const String& password);

  private:
    std::shared_ptr<UserDb> m_user_db;

    bool is_in_db(const String& username);
};
}  // namespace twodocore