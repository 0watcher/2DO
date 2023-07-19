#pragma once

#include <iostream>
#include <optional>

namespace twodo
{
    enum class ErrorCode
    {
        ok,
        panic,
        incorrect_nickname,
        already_existing_name,
        incorrect_password,
        not_match_regex,
        user_added_successfully
    };

    template<typename T>
    struct Result 
    {
    public:
        std::optional<T> m_value;
        ErrorCode m_err{ ErrorCode::ok };
    };

    template<>
    struct Result<void>
    {
    public:
        ErrorCode m_err{ ErrorCode::ok };
    };
}