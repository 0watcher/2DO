#include "utils.hpp"

namespace twodo
{
    Result<std::string, StdError> input()
    {
        std::string input{};
        std::getline(std::cin, input);
        if(std::cin.fail())
        {
            return Error<std::string, StdError>(StdError::CinError);
        }
        return Ok<std::string, StdError>(input);
    }

    Result<const std::string, StdError> hash(const std::string& password)
    {
        try
        {
            std::hash<std::string> hasher{};
            size_t hash = hasher(password);
            return Ok<const std::string, StdError>(std::to_string(hash));
        }
        catch (...)
        {
            return Error<const std::string, StdError>(StdError::HashError);
        }
    }
}