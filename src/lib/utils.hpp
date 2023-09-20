#pragma once

#include <iostream>
#include <string>

#include "result.hpp"

using String = std::string;

namespace twodo
{
String input() noexcept;

String hash(const String& str) noexcept;

class IUserInputHandler
{
   public:
    virtual String getInput() = 0;

   private:
    ~IUserInputHandler() {};
};
}  // namespace twodo
