#include <iostream>
#include <string>

#include "result.hpp"

namespace twodo
{
    enum class StdError
    {
        HashError,
        CinError
    };

    Result<std::string, StdError> input() noexcept;

    Result<std::string, StdError> hash(const std::string& str) noexcept;
}  // namespace twodo
