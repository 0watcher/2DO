#include "utils.hpp"

namespace twodo
{
    Result<std::string> input()
    {
        std::string input{};
        std::getline(std::cin, input);
        if(std::cin.fail())
        {
            return Result<std::string>{ .m_err = ErrorCode::cin_failure };
        }
        return Result<std::string>{ .m_value = input };
    }

    Result <const std::string> hash(const std::string& password)
    {
        try
        {
            std::hash<std::string> hasher{};
            size_t hash = hasher(password);
            return Result<const std::string>{std::to_string(hash)};
        }
        catch (...)
        {
            return Result<const std::string>{ .m_err = ErrorCode::hash_error };
        }
    }
}