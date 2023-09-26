#include "user.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>

#include "database.hpp"
#include "result.hpp"

namespace twodo
{

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
        return Err<User, UsrDbErr>(UsrDbErr::GetUserDataErr);
    }
    stringvec usr_data = data.value();

    return Ok<User, UsrDbErr>(
        User {std::stoi(usr_data[0]), usr_data[1], stor(usr_data[2]), usr_data[3]});
}

Result<int, UsrDbErr> UserDb::get_user_id(const String& username)
{
    auto id = m_db.select_data("users", {"id"}, {"username", username});
    if(!id)
    {
        return Err<int, UsrDbErr>(UsrDbErr::GetUserDataErr);
    }
    return Ok<int, UsrDbErr>(stoi(id.value()[0]));
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

Result<None, UsrDbErr> UserDb::update_data(const User& user)
{
    auto data = m_db.select_data("users", {"username", "role", "password"}, {"id", std::to_string(user.get_id())});
    if(!data)
    {
        return Err<None, UsrDbErr>(UsrDbErr::GetUserDataErr);
    }

    String db_username = data.value()[0];
    String db_role = data.value()[1];
    String db_password = data.value()[2];

    String id = std::to_string(user.get_id());
    String username = user.get_username();
    String role = rtos(user.get_role());
    String password = user.get_password();

    if(username != db_username)
    {
        auto result = m_db.update_data("users", {"username", username}, {"id", id});
        if(!result)
        {
            return Err<None, UsrDbErr>(UsrDbErr::UpdateDataErr);
        }
    }

    if(role != db_role)
    {
        auto result = m_db.update_data("users", {"role", role}, {"id", id});
        if(!result)
        {
            return Err<None, UsrDbErr>(UsrDbErr::UpdateDataErr);
        }
    }

    if(password != db_password)
    {
        auto result = m_db.update_data("users", {"password", password}, {"id", id});
        if(!result)
        {
            return Err<None, UsrDbErr>(UsrDbErr::UpdateDataErr);
        }
    }

    return Ok<None, UsrDbErr>({});
}   

bool UserDb::is_empty() { return m_db.is_table_empty("users"); }

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

Result<User, AuthErr> RegisterManager::singup()
{
    String username_ {};

    while (true)
    {
        m_idisplayer.msg_display("username: ");
        String username = m_ihandler.getInput();
        auto usrnm_result = username_validation(username);
        if (!usrnm_result)
        {
            m_idisplayer.err_display("Invalid name length!");
        }
        else
        {
            auto name_exists = m_udb.get_user(username);
            if (!name_exists)
            {
                username_ = username;
                break;
            }
            m_idisplayer.err_display("User with this name already exists!");
        }
    }

    while (true)
    {
        m_idisplayer.msg_display("password: ");
        String password = m_ihandler.getInput();

        auto passwd_result = password_validation(password);
        if (!passwd_result)
        {
            switch (passwd_result.err())
            {
                case AuthErr::PasswordTooShort:
                {
                    m_idisplayer.err_display("Password is too short!");
                    break;
                }
                case AuthErr::MissingLowerCase:
                {
                    m_idisplayer.err_display(
                        "Password must contain at least one lowercase letter!");
                    break;
                }
                case AuthErr::MissingUpperCase:
                {
                    m_idisplayer.err_display(
                        "Password must contain at least one uppercase letter!");
                    break;
                }
                case AuthErr::MissingNumber:
                {
                    m_idisplayer.err_display("Password must contain at least one number letter!");
                    break;
                }
                case AuthErr::MissingSpecialCharacter:
                {
                    m_idisplayer.err_display(
                        "Password must contain at least one special character!");
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
            m_udb.add_user(User {username_, usr_role, hashed_passwd});
            auto id = m_udb.get_user_id(username_);
            if(!id)
            {
                return Err<User, AuthErr>(AuthErr::DbErr);
            }
            return Ok<User, AuthErr>(
                User {id.value(), username_, usr_role, hashed_passwd});
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
        m_idisplayer.err_display("User not found!");
    }

    while (true)
    {
        String password = m_ihandler.getInput();
        if (password == user->get_password())
        {
            return Ok<User, AuthErr>(user.value());
        }
        m_idisplayer.err_display("Invalid password!");
    }
}

Result<None, AuthErr> AuthManager::auth_username()
{
    String username = m_ihandler.getInput();
    auto result = m_udb.get_user(username);
    if (!result)
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
        if (password == result.value().get_password())
        {
            return Ok<User, AuthErr>(result.value());
        }
        m_idisplayer.err_display("Invalid password!");
    }
}


}  // namespace twodo