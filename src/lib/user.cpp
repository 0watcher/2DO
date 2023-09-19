#include "user.hpp"

#include <algorithm>
#include <stdexcept>

#include "database.hpp"
#include "result.hpp"
#include "utils.hpp"

namespace twodo
{
Result<User, AuthErr> AuthManager::login() {}

Result<User, AuthErr> AuthManager::singup()
{
    String username_ {};

    while (true)
    {
        String username {};
        std::getline(std::cin, username);

        auto usrnm_result = username_validation(username);
        if (!usrnm_result)
        {
            std::cerr << "Invalid name length!";
        }
        else
        {
            auto name_exists = m_db.select_data("users", {"username"}, {"username", username});
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
        String password {};
        std::getline(std::cin, password);

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
            auto hashed_passwd = hash(password).value();
            Role usr_role {};
            
            if (m_db.is_table_empty("users"))
            {
                usr_role = Role::Admin;
            }
            else
            {
                usr_role = Role::User;
            }
            m_db.insert_data(
                "users",
                {{"username", username_}, {"role", rtos(usr_role)}, {"password", hashed_passwd}});
            auto uid = m_db.select_data("users", {"id"}, {"username", username_});
            return Ok<User, AuthErr>(
                User {stoi(uid.value()[0]), username_, usr_role, hashed_passwd});
        }
    }
}

Result<None, AuthErr> AuthManager::username_validation(const String& username) const
{
    if (username.length() < 1 || username.length() > 20)
    {
        return Err<None, AuthErr>(AuthErr::InvalidNameLength);
    }
    return Ok<None, AuthErr>({});
}

Result<None, AuthErr> AuthManager::password_validation(const String& password) const
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

Role stor(const String& roleStr)
{
    static const std::map<std::string, Role> roleMap = {{"User", Role::User},
                                                        {"Admin", Role::Admin}};

    auto it = roleMap.find(roleStr);
    if (it != roleMap.end())
    {
        return it->second;
    }
    else
    {
        throw std::logic_error("Invalid role string!");
    }
}

}  // namespace twodo