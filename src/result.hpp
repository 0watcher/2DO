#pragma once

#include <iostream>
#include <optional>

namespace twodo
{
    enum class ErrorCode
    {
        Ok,
        Panic,
        InvalidInput,
        IncorrectNickname,
        AlreadyExistingName,
        IncorrectPassword,
        CinFailure,
        DbError,
        DbOpenFailure
    };

    template<typename T>
    class Result
    {
    public:
        static Result<T> Ok(T value)
        {
            return Result<T>(std::move(value), ErrorCode::Ok);
        }

        static Result<T> Error(ErrorCode error)
        {
            return Result<T>(std::nullopt, error);
        }

        explicit operator bool() const
        {
            return m_err == ErrorCode::Ok;
        }

        T& value()
        {
            if (*this)
            {
                return *m_value;
            }
            throw std::runtime_error("Can't unwrap the value!");
        }

        ErrorCode error() const
        {
            return m_err;
        }

    private:
        Result() = delete;
        Result(std::optional<T> value, ErrorCode err)
            : m_value(std::move(value)), m_err(err) {}

        std::optional<T> m_value{};
        ErrorCode m_err{};
    };

    template<>
    class Result<void>
    {
    public:
        ErrorCode error() const
        {
            return m_err;
        }

        static Result<void> Ok()
        {
            return Result<void>(ErrorCode::Ok);
        }

        static Result<void> Error(ErrorCode error)
        {
            return Result<void>(error);
        }

    private:
        ErrorCode m_err{ ErrorCode::Ok };
        explicit Result(ErrorCode err)
            : m_err(err) {}
    };
}