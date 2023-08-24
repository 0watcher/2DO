#include <optional>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

#include "result.hpp"
#include "task.hpp"
#include "ISerializable.hpp"

using String = std::string;

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

    class User
    {
    public:
        User(const int& id, const String nickname, const Role& role)
            : m_id(id), m_nickname(nickname), m_role(role)
        {
        }

        int get_id() const { return m_id; }
        void set_id(const int& id) { m_id = id; }
        String get_nickname() const { return m_nickname; }
        void set_nickname(const String& nickname) { m_nickname = nickname; }
        Role get_role() const { return m_role; }
        Role set_role(const Role& role) { m_role = role; }

    private:
        int m_id{};
        String m_nickname{};
        Role m_role{};
    };

    class Login
    {
    public:
        Login() = default;

        Result<std::string, LoginError> nickname(const String& nickname) const;
        Result<std::string, LoginError> password(const String& password);

    private:
        const bool isPasswordCorrect(const String& password) const;
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