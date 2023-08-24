#include <optional>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

#include "result.hpp"
#include "task.hpp"

namespace twodo
{
    enum class UserError
    {

    };

    enum class LoginError
    {
        IncorrectNickname,
        AlreadyExistingName,
        IncorrectPassword
    };

    enum class Role
    {
        Admin,
        User
    };

    struct User
    {
    public:
        User(const int& id, const std::string& nickname, const Role& role)
            : m_id(id), m_nickname(nickname), m_role(role)
        {
        }

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

        Result<std::string, LoginError> nickname(const std::string& nickname) const;
        Result<std::string, LoginError> password(const std::string& password);

    private:
        const bool isPasswordCorrect(const std::string& password) const;
    };

    class UserManager
    {
    public:
        UserManager() = default;

        Result<None, UserError> add_user() const;
        Result<None, UserError> delete_user() const;

        Result<None, UserError> modify_data();
    };

}  // namespace twodo