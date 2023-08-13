#include <iostream>
#include <string>

#include "result.hpp"

namespace twodo
{
    Result<std::string, StdError> input();

    Result<const std::string, StdError> hash(const std::string& password);
}
