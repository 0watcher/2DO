#include "user.hpp"
#include "utils.hpp"

#include <algorithm>

namespace twodo
{
    Result<std::string, LoginError> Login::nickname(const std::string& nickname) const
    {
        if (nickname.length() < 1 || nickname.length() > 18)
        {
            return Error<std::string, LoginError>(LoginError::IncorrectNickname);
        }

        return Ok<std::string, LoginError>(nickname);
    }

    Result<std::string, LoginError> Login::password(const std::string& password)
    {
        if (!isPasswordCorrect(password))
        {
            return Error<std::string, LoginError>(LoginError::IncorrectPassword);
        }

        auto result = hash(password);
        if(result.error() == StdError::HashError)
        {
            std::cerr << "Failed while hashing values";
            std::exit(1);
        }

        const std::string hashed_password = result.value();

        return Ok<std::string, LoginError>(hashed_password);
    }

    const bool Login::isPasswordCorrect(const std::string& password) const
    {
        const std::regex regex("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[@$!%*#?&_])[a-zA-Z\\d@$!%*#?&_]{8,}$");

        return std::regex_match(password, regex);
    }

    Result<None, UserError> UserManager::add_user() const
    {
        return Ok<UserError>();
    }

    Result<None, UserError> UserManager::delete_user() const
    {
        return Ok<UserError>();
    }

    Result<None, UserError> UserManager::modify_data(
        const int& user_id,
        const std::optional<std::string>& nickname,
        const std::optional<std::string>& password,
        const std::optional<Role>& role)
        const
    {
        return Ok<UserError>();
    }

}
