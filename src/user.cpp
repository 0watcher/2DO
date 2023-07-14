#include "user.hpp"

#include <algorithm>

namespace twodo
{
    const Result<std::string> Login::nickname() const
    {
        std::string nickname{};
        std::getline(std::cin, nickname);

        if(isUserInDb(nickname))
        {
            return Result<std::string>(ErrorCode::already_existing_name);
        }

        if(nickname.length() > 10 || nickname.length() <= 5)
        {
            return Result<std::string>(ErrorCode::incorrect_nickname);
        }

        return Result<std::string>(nickname);
    }

    const Result<std::string> Login::password()
    {
        std::string password{};
        std::getline(std::cin, password);

        if(!isPasswordCorrect(password))
        {
            return Result<std::string>(ErrorCode::incorrect_nickname);
        }

        const std::string hashed_password = hash(password);

        return Result<std::string>(hashed_password);
    }

    const std::string Login::hash(const std::string& password)
    {
        std::hash<std::string> hasher{};
        size_t hash = hasher(password);
        return std::to_string(hash);
    }

    const bool Login::isPasswordCorrect(const std::string& password) const
    {
        const std::regex regex("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[@$!%*#?&])[a-zA-Z\\d@$!%*#?&]{8,}$");
        
        return std::regex_match(password, regex);
    }

    const bool Login::isUserInDb(const std::string& nickname) const
    {
        return false;
    }
    
    Result<void> UserManager::add_user() const
    {
        return Result<void>(ErrorCode::overall_err);
    }

    Result<void> UserManager::delete_user() const
    {
        return Result<void>(ErrorCode::overall_err);
    }

    Result<void> UserManager::modify_data(const int& user_id, const std::optional<std::string>& nickname, const std::optional<std::string>& password) const
    {
        return Result<void>(ErrorCode::overall_err);
    }
}