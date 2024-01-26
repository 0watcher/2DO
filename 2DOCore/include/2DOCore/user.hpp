#pragma once

#include <optional>

#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <Utils/database.hpp>
#include <Utils/result.hpp>
#include <Utils/type.hpp>
#include <Utils/util.hpp>
#include "2DOCore/task.hpp"

namespace SQL = SQLite;
namespace tdl = twodoutils;

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
         StringView password)
        : m_user_id{user_id},
          m_username{username},
          m_role{role},
          m_password{password} {}

    User(unsigned int user_id,
         StringView username,
         String role,
         StringView password)
        : m_user_id{user_id},
          m_username{username},
          m_role{stor(role)},
          m_password{password} {}

    User(StringView username, Role role, StringView password)
        : m_username{username}, m_role{role}, m_password{password} {}

    bool operator==(const User& other) const {
        return m_user_id == other.m_user_id && m_username == other.m_username &&
               m_role == other.m_role && m_password == other.m_password;
    }

    [[nodiscard]] unsigned int id() const { return m_user_id.value(); }
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
    void set_password(StringView passwd) { m_password = passwd; }

  private:
    std::optional<unsigned int> m_user_id{std::nullopt};
    String m_username{};
    Role m_role{};
    String m_password{};

    [[nodiscard]] Role stor(const String& role_str) const;
    [[nodiscard]] String rtos(Role role) const;
};

class [[nodiscard]] UserDb : protected tdl::Database<User> {
  public:
    UserDb(const UserDb&) = delete;
    UserDb& operator=(const UserDb&) = delete;
    UserDb(UserDb&& other) = default;
    UserDb& operator=(UserDb&& other) = default;

    UserDb(StringView db_filepath);

    tdl::Result<User, tdl::DbError> get_object_by_id(
        unsigned int id) const noexcept override;

    tdl::Result<User, tdl::DbError> find_object_by_unique_column(
        const String& column_value) const noexcept;

    tdl::Result<Vector<User>, tdl::DbError> get_all_objects()
        const noexcept override;

    bool is_table_empty() const noexcept override;

    tdl::Result<void, tdl::DbError> add_object(User& user) noexcept override;

    tdl::Result<void, tdl::DbError> update_object(
        const User& user) const noexcept override;

    tdl::Result<void, tdl::DbError> delete_object(
        const User& user) const noexcept override;
};
}  // namespace twodocore