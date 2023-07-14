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
        User(const int& id_, const std::string& nickname_, const std::string& password_)
            : m_id(id_), m_nickname(nickname_), m_password(password_) {}

        int get_id() const { return m_id; }
        void set_id(const int& id) { m_id = id; }
        std::string get_nickname() const { return m_nickname; }
        void set_nickname(const std::string& nickname) { m_nickname = nickname; }
        std::string get_password() const { return m_password; }
        void set_password(const std::string& password) { m_password = password; }
        Role get_role() const { return m_role; }
        Role set_role(const Role& role) { m_role = role; }

    private:
        int m_id{};
        std::string m_nickname{};
        std::string m_password{};
        Role m_role{};
    };

    class Login
    {
    public:
        Login() = default;

        const Result<std::string> nickname() const;
        const Result<std::string> password();

    private:
        const std::string hash(const std::string& password);

        const bool isPasswordCorrect(const std::string& password) const;
        const bool isUserInDb(const std::string& nickname) const;
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
            const std::optional<std::string>& password)
            const;
    };

}