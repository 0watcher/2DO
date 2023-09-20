#include "user.hpp"

#include <algorithm>
#include <stdexcept>

#include "database.hpp"
#include "result.hpp"

namespace twodo
{
Result<User, AuthErr> AuthManager::login()
{
    std::optional<User> user {};

    while (true)
    {
        String username = m_ihandler.getInput();
        auto result = m_udb.get_user(username);
        if (result)
        {
            user = result.value();
        }
        std::cerr << "User not found!";
    }

    while (true)
    {
        String password = m_ihandler.getInput();
        if (password == user->get_password())
        {
            return Ok<User, AuthErr>(user.value());
        }
        std::cerr << "Invalid password!";
    }
}

Result<None, AuthErr> AuthManager::auth_username()
{
    String username = m_ihandler.getInput();
    auto result = m_udb.get_user(username);
    if(!result)
    {
        return Err<None, AuthErr>(AuthErr::UserNotFound);
    }
    return Ok<None, AuthErr>({});
}

Result<User, AuthErr> AuthManager::auth_password(const String& username)
{
    auto result = m_udb.get_user(username);
    if (!result)
    {
        return Err<User, AuthErr>(AuthErr::UserNotFound);
    }
    while (true)
    {
        String password = m_ihandler.getInput();
        if(password == result.value().get_password())
        {
            return Ok<User, AuthErr>(result.value());
        }
        std::cerr << "Invalid password!";
    }
}

Result<User, AuthErr> RegisterManager::singup()
{
    String username_ {};

    while (true)
    {
        std::cout << "username: ";
        String username = m_ihandler.getInput();
        auto usrnm_result = username_validation(username);
        if (!usrnm_result)
        {
            std::cerr << "Invalid name length!";
        }
        else
        {
            auto name_exists = m_udb.get_user(username);
            if (!name_exists)
            {
                username_ = username;
                break;
            }
            std::cerr << "User with this name already exists!";
        }
    }

    while (true)
    {
        std::cout << "password: ";
        String password = m_ihandler.getInput();

        auto passwd_result = password_validation(password);
        if (!passwd_result)
        {
            switch (passwd_result.err())
            {
                case AuthErr::PasswordTooShort:
                {
                    std::cerr << "Password is too short!";
                    break;
                }
                case AuthErr::MissingLowerCase:
                {
                    std::cerr << "Password must contain at least one lowercase letter!";
                    break;
                }
                case AuthErr::MissingUpperCase:
                {
                    std::cerr << "Password must contain at least one uppercase letter!";
                    break;
                }
                case AuthErr::MissingNumber:
                {
                    std::cerr << "Password must contain at least one number letter!";
                    break;
                }
                case AuthErr::MissingSpecialCharacter:
                {
                    std::cerr << "Password must contain at least one special character!";
                    break;
                }
            }
        }
        else
        {
            auto hashed_passwd = hash(password);
            Role usr_role {};

            if (m_udb.is_empty())
            {
                usr_role = Role::Admin;
            }
            else
            {
                usr_role = Role::User;
            }
            m_udb.add_user(User{username_, usr_role, hashed_passwd});
            auto user = m_udb.get_user(username_);
            return Ok<User, AuthErr>(
                User {user.value().get_id(), username_, usr_role, hashed_passwd});
        }
    }
}

Result<None, AuthErr> RegisterManager::username_validation(const String& username) const
{
    if (username.length() < 1 || username.length() > 20)
    {
        return Err<None, AuthErr>(AuthErr::InvalidNameLength);
    }
    return Ok<None, AuthErr>({});
}

Result<None, AuthErr> RegisterManager::password_validation(const String& password) const
{
    std::regex upper_case_expression {"[A-Z]+"};
    std::regex lower_case_expression {"[a-z]+"};
    std::regex number_expression {"[0-9]+"};
    std::regex special_char_expression {"[!@#$%^&*()_+\\-=\\[\\]{};:\\\",<.>/?]+"};

    if (password.length() >= 8)
    {
        return Err<None, AuthErr>(AuthErr::PasswordTooShort);
    }
    if (std::regex_search(password, upper_case_expression))
    {
        return Err<None, AuthErr>(AuthErr::MissingUpperCase);
    }
    if (std::regex_search(password, lower_case_expression))
    {
        return Err<None, AuthErr>(AuthErr::MissingLowerCase);
    }
    if (std::regex_search(password, number_expression))
    {
        return Err<None, AuthErr>(AuthErr::MissingNumber);
    }
    if (std::regex_search(password, special_char_expression))
    {
        return Err<None, AuthErr>(AuthErr::MissingSpecialCharacter);
    }

    return Ok<None, AuthErr>({});
}

UserDb::UserDb()
{
    m_db.create_table("users", {{"id", "INT PRIMARY KEY AUTOINCREMENT"},
                                {"username", "TEXT"},
                                {"role", "TEXT"},
                                {"password", "TEXT"}});
}

Result<User, UsrDbErr> UserDb::get_user(const String& username) noexcept
{
    auto data =
        m_db.select_data("users", {"id", "username", "role", "password"}, {"username", username});
    if (!data)
    {
        return Err<User, UsrDbErr>(UsrDbErr::GetUserErr);
    }
    stringvec usr_data = data.value();

    return Ok<User, UsrDbErr>(
        User {std::stoi(usr_data[0]), usr_data[1], stor(usr_data[2]), usr_data[3]});
}

Result<None, UsrDbErr> UserDb::add_user(const User& user)
{
    auto result = m_db.insert_data("users", {{"username", user.get_username()},
                                             {"role", rtos(user.get_role())},
                                             {"password", user.get_password()}});
    if (!result)
    {
        return Err<None, UsrDbErr>(UsrDbErr::AddUserErr);
    }
    return Ok<None, UsrDbErr>({});
}

Result<None, UsrDbErr> UserDb::delete_user(const String& username)
{
    auto result = m_db.delete_data("users", {"username", username});
    if (!result)
    {
        return Err<None, UsrDbErr>(UsrDbErr::DeleteUserErr);
    }
    return Ok<None, UsrDbErr>({});
}

Result<None, UsrDbErr> UserDb::update_data(const User& user) {}

bool UserDb::is_empty()
{
    return m_db.is_table_empty("users");
}

String rtos(Role role)
{
    switch (role)
    {
        case Role::Admin:
            return "Admin";
            break;
        case Role::User:
            return "User";
            break;
        default:
            throw std::logic_error("Invalid role enum!");
            break;
    }
}

Role stor(const String& role_str)
{
    static const std::map<std::string, Role> role_map = {{"User", Role::User},
                                                         {"Admin", Role::Admin}};

    auto it = role_map.find(role_str);
    if (it != role_map.end())
    {
        return it->second;
    }
    else
    {
        throw std::logic_error("Invalid role string!");
    }
}

}  // namespace twodo