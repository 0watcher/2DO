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
}