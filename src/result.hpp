#pragma once

#include <iostream>
#include <optional>
#include <stdexcept>

namespace twodo
{

    enum class ErrorCode
    {
        ok,
        panic,
        invalid_input,
        incorrect_nickname,
        already_existing_name,
        incorrect_password,
        cin_failure,
    };

    template<typename T>
    struct Result
    {
    public:
        std::optional<T> m_value;
        ErrorCode m_err{ ErrorCode::ok };

        T value() const
        {
            if (m_value.has_value() && m_err == ErrorCode::ok)
            {
                return m_value.value();
            }
            else
            {
                throw std::runtime_error("Can't unwrap the value!");
            }
        }

        ErrorCode err() const
        {
            return m_err;
        }
    };

    template<>
    struct Result<void>
    {
    public:
        ErrorCode m_err{ ErrorCode::ok };

        ErrorCode err() const
        {
            return m_err;
        }
    };
}