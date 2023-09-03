#include "user.hpp"
#include <algorithm>
#include "utils.hpp"

namespace twodo
{
Result<String, LoginError> Login::nickname(const String& nickname) const
{
    if (nickname.length() < 1 || nickname.length() > 18)
    {
        return Err<String, LoginError>(LoginError::IncorrectNickname);
    }

    return Ok<String, LoginError>(nickname);
}

Result<String, LoginError> Login::password(const String& password)
{
    if (!is_password_correct(password))
    {
        return Err<String, LoginError>(LoginError::IncorrectPassword);
    }

    auto result = hash(password);
    if (!result)
    {
        std::cerr << "Failed while hashing values";
        std::exit(1);
    }

    String hashed_password = result.value();

    return Ok<String, LoginError>(std::move(hashed_password));
}

const bool Login::is_password_correct(const String& password) const
{
    std::regex upper_case_expression{ "[A-Z]+" };
    std::regex lower_case_expression{ "[a-z]+" };
    std::regex number_expression{ "[0-9]+" };
    std::regex special_char_expression{ "[!@#$%^&*()_+\\-=\\[\\]{};:\\\",<.>/?]+" };

    bool has_upper = std::regex_search(password, upper_case_expression);
    bool has_lower = std::regex_search(password, lower_case_expression);
    bool has_number = std::regex_search(password, number_expression);
    bool has_special = std::regex_search(password, special_char_expression);
    bool has_required_legth = password.length() >= 8;

    return has_upper && has_lower && has_number && has_special && has_required_legth;
}

Result<None, UserError> UserManager::add_user() const { return Ok<None, UserError>({}); }

Result<None, UserError> UserManager::delete_user() const { return Ok<None, UserError>({}); }

Result<None, UserError> UserManager::modify_data() { return Ok<None, UserError>({}); }
}  // namespace twodo
