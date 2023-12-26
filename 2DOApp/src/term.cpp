#include "2DOApp/term.hpp"

namespace twodo
{
[[nodiscard]] tdc::Result<tdc::User, AuthErr> RegisterManager::singup()
{
    auto username_ = std::string();

    while (true)
    {
        m_idisplayer->msg_display("username: ");
        auto username = m_ihandler->get_input();
        auto usrnm_result = username_validation(username);

        if (!usrnm_result)
        {
            m_idisplayer->err_display("Invalid name length!\n");
        }
        else
        {
            auto name_exists = m_udb->get_user(username);
            if (!name_exists)
            {
                username_ = username;
                break;
            }
            m_idisplayer->err_display("User with this name already exists!\n");
        }
    }

    while (true)
    {
        m_idisplayer->msg_display("password: ");
        auto password = m_ihandler->get_input();

        auto passwd_result = password_validation(password);
        if (!passwd_result)
        {
            switch (passwd_result.err())
            {
                case AuthErr::PasswordTooShort:
                {
                    m_idisplayer->err_display("Password is too short!\n");
                    break;
                }
                case AuthErr::MissingLowerCase:
                {
                    m_idisplayer->err_display(
                        "Password must contain at least one lowercase letter!\n");
                    break;
                }
                case AuthErr::MissingUpperCase:
                {
                    m_idisplayer->err_display(
                        "Password must contain at least one uppercase letter!\n");
                    break;
                }
                case AuthErr::MissingNumber:
                {
                    m_idisplayer->err_display(
                        "Password must contain at least one number letter!\n");
                    break;
                }
                case AuthErr::MissingSpecialCharacter:
                {
                    m_idisplayer->err_display(
                        "Password must contain at least one special character!\n");
                    break;
                }
            }
        }
        else
        {
            auto hashed_passwd = tdc::hash(password);
            tdc::Role usr_role {};

            if (m_udb->is_empty())
            {
                usr_role = tdc::Role::Admin;
            }
            else
            {
                usr_role = tdc::Role::User;
            }
            m_udb->add_user(tdc::User {username_, usr_role, hashed_passwd});
            auto id = m_udb->get_user_id(username_);
            if (!id)
            {
                return tdc::Err<tdc::User, AuthErr>(AuthErr::DbErr);
            }
            return tdc::Ok<tdc::User, AuthErr>(tdc::User {id.value(), username_, usr_role, hashed_passwd});
        }
    }
}

[[nodiscard]] tdc::Result<tdc::None, AuthErr> RegisterManager::username_validation(
    std::string_view username) const
{
    if (username.length() < 1 || username.length() > 20)
    {
        return tdc::Err<tdc::None, AuthErr>(AuthErr::InvalidNameLength);
    }
    return tdc::Ok<tdc::None, AuthErr>({});
}

[[nodiscard]] tdc::Result<tdc::None, AuthErr> RegisterManager::password_validation(
    const String& password) const
{
    std::regex upper_case_expression {"[A-Z]+"};
    std::regex lower_case_expression {"[a-z]+"};
    std::regex number_expression {"[0-9]+"};
    std::regex special_char_expression {"[!@#$%^&*()_+\\-=\\[\\]{};:\\\",<.>/?]+"};

    if (password.length() < 8)
    {
        return tdc::Err<tdc::None, AuthErr>(AuthErr::PasswordTooShort);
    }
    if (!std::regex_search(password, upper_case_expression))
    {
        return tdc::Err<tdc::None, AuthErr>(AuthErr::MissingUpperCase);
    }
    if (!std::regex_search(password, lower_case_expression))
    {
        return tdc::Err<tdc::None, AuthErr>(AuthErr::MissingLowerCase);
    }
    if (!std::regex_search(password, number_expression))
    {
        return tdc::Err<tdc::None, AuthErr>(AuthErr::MissingNumber);
    }
    if (!std::regex_search(password, special_char_expression))
    {
        return tdc::Err<tdc::None, AuthErr>(AuthErr::MissingSpecialCharacter);
    }

    return tdc::Ok<tdc::None, AuthErr>({});
}

[[nodiscard]] tdc::Result<tdc::User, AuthErr> AuthManager::login()
{
    std::optional<tdc::User> user {std::nullopt};

    int tries = 3;

    while (true)
    {
        if (tries <= 0)
        {
            return tdc::Err<tdc::User, AuthErr>(AuthErr::AllTriesExhausted);
        }

        m_idisplayer->msg_display("username: ");
        String username = m_ihandler->get_input();
        auto result = m_udb->get_user(username);
        if (!result)
        {
            m_idisplayer->err_display("User not found!\n");
            tries--;
        }
        else
        {
            user = result.value();
            break;
        }
    }

    tries = 3;

    while (true)
    {
        if (tries <= 0)
        {
            return tdc::Err<tdc::User, AuthErr>(AuthErr::AllTriesExhausted);
        }

        m_idisplayer->msg_display("password: ");
        String password = m_ihandler->get_input();
        if (tdc::hash(password) == user->get_password())
        {
            return tdc::Ok<tdc::User, AuthErr>(std::move(user.value()));
        }
        m_idisplayer->err_display("Invalid password!\n");
        tries--;
    }
}

[[nodiscard]] tdc::Result<tdc::None, AuthErr> AuthManager::auth_username()
{
    m_idisplayer->msg_display("username: ");
    String username = m_ihandler->get_input();
    auto result = m_udb->get_user(username);
    if (!result)
    {
        return tdc::Err<tdc::None, AuthErr>(AuthErr::UserNotFound);
    }
    return tdc::Ok<tdc::None, AuthErr>({});
}

[[nodiscard]] tdc::Result<tdc::User, AuthErr> AuthManager::auth_password(const String& username)
{
    auto result = m_udb->get_user(username);
    if (!result)
    {
        return tdc::Err<tdc::User, AuthErr>(AuthErr::UserNotFound);
    }
    while (true)
    {
        m_idisplayer->msg_display("password: ");
        String password = m_ihandler->get_input();
        if (password == result.value().get_password())
        {
            return tdc::Ok<tdc::User, AuthErr>(std::move(result.value()));
        }
        m_idisplayer->err_display("Invalid password!\n");
    }
}
}  // namespace app