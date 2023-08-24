#pragma once

#include <string>

#include "result.hpp"

using String = std::string;

namespace twodo
{
enum SerializeErr
{
    SerErr
};

class ISerializable
{
   public:
    virtual Result<String, SerializeErr> sql_serialize() = 0;
};
}  // namespace twodo