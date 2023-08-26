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
    if (!isPasswordCorrect(password))
    {
        return Err<String, LoginError>(LoginError::IncorrectPassword);
    }

    auto result = hash(password);
    if (result.error() == StdError::HashError)
    {
        std::cerr << "Failed while hashing values";
        std::exit(1);
    }

    String hashed_password = result.value();

    return Ok<String, LoginError>(hashed_password);
}

const bool Login::isPasswordCorrect(const String& password) const
{
    const std::regex regex{
        "^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[@$!%*#?&_])[a-zA-Z\\d@$!%*#?&_]{8,}$"};

    return std::regex_match(password, regex);
}

Result<None, UserError> UserManager::add_user() const { return Ok<None, UserError>({}); }

Result<None, UserError> UserManager::delete_user() const { return Ok<None, UserError>({}); }

Result<None, UserError> UserManager::modify_data() { return Ok<None, UserError>({}); }
}  // namespace twodo
