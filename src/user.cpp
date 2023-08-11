#include "user.hpp"
#include "utils.hpp"

#include <algorithm>

namespace twodo
{
    Result<std::string> Login::nickname(const std::string& nickname) const
    {
        if (nickname.length() < 1 || nickname.length() > 18)
        {
            return Result<std::string>::Error(ErrorCode::IncorrectNickname);
        }

        return Result<std::string>::Ok(nickname);
    }

    Result<std::string> Login::password(const std::string& password)
    {
        if (!isPasswordCorrect(password))
        {
            return Result<std::string>::Error(ErrorCode::IncorrectPassword);
        }

        auto result = hash(password);
        if(result.error() == ErrorCode::Panic)
        {
            std::cerr << "Failed while hashing values";
            std::exit(1);
        }

        const std::string hashed_password = result.value();

        return Result<std::string>::Ok(hashed_password);
    }

    const bool Login::isPasswordCorrect(const std::string& password) const
    {
        const std::regex regex("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[@$!%*#?&_])[a-zA-Z\\d@$!%*#?&_]{8,}$");

        return std::regex_match(password, regex);
    }

    Result<void> UserManager::add_user() const
    {
        return Result<void>::Ok();
    }

    Result<void> UserManager::delete_user() const
    {
        return Result<void>::Ok();
    }

    Result<void> UserManager::modify_data(
        const int& user_id,
        const std::optional<std::string>& nickname,
        const std::optional<std::string>& password,
        const std::optional<Role>& role)
        const
    {
        return Result<void>::Ok();
    }

}
