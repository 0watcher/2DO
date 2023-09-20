#include "utils.hpp"

namespace twodo
{
String input() noexcept
{
    String input {};
    std::getline(std::cin, input);
    if (std::cin.fail())
    {
        throw;
    }
    return input;
}

String hash(const String& str) noexcept
{
    std::hash<String> hasher {};
    auto hashed_value = std::to_string(hasher(str));
    if (hashed_value.empty())
    {
        throw;
    }
    return hashed_value;
}
}  // namespace twodo