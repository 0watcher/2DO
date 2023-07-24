#include <iostream>
#include <string>

#include "result.hpp"

namespace twodo
{
    Result<std::string> input();

    Result<const std::string> hash(const std::string& password);
}
