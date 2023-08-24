#include "user.hpp"

#include <algorithm>

#include "utils.hpp"

namespace twodo
{
Result<std::string, LoginError> Login::nickname(const std::string& nickname) const
{
    if (nickname.length() < 1 || nickname.length() > 18)
    {
        return Err<std::string, LoginError>(LoginError::IncorrectNickname);
    }

    return Ok<std::string, LoginError>(nickname);
}

Result<std::string, LoginError> Login::password(const std::string& password)
{
    if (!isPasswordCorrect(password))
    {
        return Err<std::string, LoginError>(LoginError::IncorrectPassword);
    }

    auto result = hash(password);
    if (result.error() == StdError::HashError)
    {
        std::cerr << "Failed while hashing values";
        std::exit(1);
    }

    std::string hashed_password = result.value();

    return Ok<std::string, LoginError>(hashed_password);
}

const bool Login::isPasswordCorrect(const std::string& password) const
{
    const std::regex regex(
        "^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[@$!%*#?&_])[a-zA-Z\\d@$!%*#?&_]{8,}$");

    return std::regex_match(password, regex);
}

Result<None, UserError> UserManager::add_user() const { return Ok<None, UserError>({}); }

Result<None, UserError> UserManager::delete_user() const { return Ok<None, UserError>({}); }

Result<None, UserError> UserManager::modify_data() { return Ok<None, UserError>({}); }
}  // namespace twodo
