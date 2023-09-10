#include "user.hpp"

#include <algorithm>
#include <stdexcept>

#include "database.hpp"
#include "result.hpp"
#include "utils.hpp"

namespace twodo
{
Result<String, AuthErr> AuthenticationManager::auth_nickname(const String& nickname) const
{
    if (nickname.length() < 1 || nickname.length() > 18)
    {
        return Err<String, AuthErr>(AuthErr::IncorrectNickname);
    }

    return Ok<String, AuthErr>(nickname);
}

Result<String, AuthErr> AuthenticationManager::auth_password(const String& password)
{
    if (!is_passwd_format_valid(password))
    {
        return Err<String, AuthErr>(AuthErr::IncorrectPassword);
    }

    auto result = hash(password);
    if (!result)
    {
        std::cerr << "Failed while hashing values";
        std::exit(1);
    }

    String hashed_password = result.value();

    return Ok<String, AuthErr>(std::move(hashed_password));
}

const bool AuthenticationManager::is_passwd_format_valid(const String& password) const
{
    std::regex upper_case_expression {"[A-Z]+"};
    std::regex lower_case_expression {"[a-z]+"};
    std::regex number_expression {"[0-9]+"};
    std::regex special_char_expression {"[!@#$%^&*()_+\\-=\\[\\]{};:\\\",<.>/?]+"};

    bool has_upper = std::regex_search(password, upper_case_expression);
    bool has_lower = std::regex_search(password, lower_case_expression);
    bool has_number = std::regex_search(password, number_expression);
    bool has_special = std::regex_search(password, special_char_expression);
    bool has_required_legth = password.length() >= 8;

    return has_upper && has_lower && has_number && has_special && has_required_legth;
}

UserDb::UserDb()
{
    m_db.create_table(
        "users", {{"id", "INT"}, {"username", "TEXT"}, {"role", "TEXT"}, {"password", "TEXT"}});
}

Result<User, UsrDbErr> UserDb::get_user(const String& username) noexcept
{
    auto data =
        m_db.select_data("users", {"id", "username", "role", "password"}, {"username", username});
    if (!data)
    {
        return Err<User, UsrDbErr>(UsrDbErr::CannotGetUser);
    }
    stringvec usr_data = data.value();

    return Ok<User, UsrDbErr>(
        User {std::stoi(usr_data[0]), usr_data[1], stor(usr_data[2]), usr_data[3]});
}

Result<None, UsrDbErr> UserDb::add_user(const User& user) const
{

}

Role UserDb::stor(const std::string& roleStr)
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
        throw std::runtime_error("Invalid role string");
    }
}

}  // namespace twodo