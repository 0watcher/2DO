#include "utils.hpp"

namespace twodo
{
    Result<std::string> input()
    {
        std::string input{};
        std::getline(std::cin, input);
        if(std::cin.fail())
        {
            return Result<std::string>::Error(ErrorCode::CinFailure);
        }
        return Result<std::string>::Ok(input);
    }

    Result <const std::string> hash(const std::string& password)
    {
        try
        {
            std::hash<std::string> hasher{};
            size_t hash = hasher(password);
            return Result<const std::string>::Ok(std::to_string(hash));
        }
        catch (...)
        {
            return Result<const std::string>::Error(ErrorCode::Panic);
        }
    }
}