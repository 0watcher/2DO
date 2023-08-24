#include "utils.hpp"

namespace twodo
{
Result<std::string, StdError> input()
{
    std::string input {};
    std::getline(std::cin, input);
    if (std::cin.fail())
    {
        return Err<std::string, StdError>(StdError::CinError);
    }
    return Ok<std::string, StdError>(std::move(input));
}

Result<std::string, StdError> hash(const std::string& password)
{
    try
    {
        std::hash<std::string> hasher {};
        size_t hash = hasher(password);
        return Ok<std::string, StdError>(std::to_string(hash));
    }
    catch (...)
    {
        return Err<std::string, StdError>(StdError::HashError);
    }
}
}  // namespace twodo