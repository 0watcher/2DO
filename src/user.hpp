#include "task.hpp"

#include "result.hpp"

#include <optional>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

namespace twodo
{
    enum class Role
    {
        ADMIN,
        USER
    };

    struct User
    {
    public:
        User(const int& id, const std::string& nickname, const Role& role)
            : m_id(id), m_nickname(nickname), m_role(role) {}

        int get_id() const { return m_id; }
        void set_id(const int& id) { m_id = id; }
        std::string get_nickname() const { return m_nickname; }
        void set_nickname(const std::string& nickname) { m_nickname = nickname; }
        Role get_role() const { return m_role; }
        Role set_role(const Role& role) { m_role = role; }

    private:
        int m_id{};
        std::string m_nickname{};
        Role m_role{};
    };

    class Login
    {
    public:
        Login() = default;

        Result<const std::string> nickname(const std::string& nickname) const;
        Result<const std::string> password(const std::string& password);
    private:
        const bool isPasswordCorrect(const std::string& password) const;
    };

    class UserManager
    {
    public:
        UserManager() = default;

        Result<void> add_user() const;
        Result<void> delete_user() const;

        Result<void> modify_data(
            const int& user_id,
            const std::optional<std::string>& nickname,
            const std::optional<std::string>& password,
            const std::optional<Role>& role)
            const;
    };

}