#include "utils.hpp"

namespace twodo
{
Result<std::string, StdError> input() noexcept
{
    std::string input {};
    std::getline(std::cin, input);
    if (std::cin.fail())
    {
        return Err<std::string, StdError>(StdError::CinError);
    }
    return Ok<std::string, StdError>(std::move(input));
}

Result<std::string, StdError> hash(const std::string& password) noexcept
{
    std::hash<std::string> hasher {};
    auto hash = std::to_string(hasher(password));
    if (hash.empty())
    {
        return Err<std::string, StdError>(StdError::HashError);
    }
    return Ok<std::string, StdError>(hash);
}
}  // namespace twodo