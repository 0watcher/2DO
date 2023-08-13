#pragma once

#include <iostream>
#include <variant>

namespace twodo
{

    enum class RunError
    {
        ExitFailure
    };

    enum class StdError
    {
        HashError,
        CinError
    };

    enum class UserError
    {

    };

    enum class LoginError
    {
        IncorrectNickname,
        AlreadyExistingName,
        IncorrectPassword
    };

    enum class InputError
    {
        InvalidInput,
    };

    enum class DbError
    {
        DbError,
        OpenFailure,
        TableCreateFailure,
        InsertFailure,
        SelectFailure,
        UpdateFailure
    };

    struct None {};

    template <typename T, typename E>
    class Result
    {
    public:
        const T& value() const
        {
            return std::get<T>(m_storage);
        }

        const E& error() const
        {
            return std::get<E>(m_storage);
        }

        explicit operator bool() const
        {
            return !std::holds_alternative<E>(m_storage);
        }

    private:
        template <typename U, typename V>
        friend Result<U, V> Ok(const U& value);

        template <typename U, typename V>
        friend Result<U, V> Ok(U&& value);

        template <typename V>
        friend Result<None, V> Ok();

        template <typename U, typename V>
        friend Result<U, V> Error(const V& error);

        template <typename U, typename V>
        friend Result<U, V> Error(V&& error);

        Result(const T& value) : m_storage(value) {}
        Result(T&& value) : m_storage(std::move(value)) {}
        Result(const E& error) : m_storage(error) {}
        Result(E&& error) : m_storage(std::move(error)) {}
        Result(None&) {}

        std::variant<T, E> m_storage{};
    };

    template <typename T, typename E>
    Result<T, E> Ok(const T& value)
    {
        return Result<T, E>(value);
    }

    template <typename T, typename E>
    Result<T, E> Ok(T&& value)
    {
        return Result<T, E>(std::move(value));
    }

    template <typename E>
    Result<None, E> Ok()
    {
        return Result<None, E>(None{});
    }

    template <typename T, typename E>
    Result<T, E> Error(const E& error)
    {
        return Result<T, E>(error);
    }

    template <typename T, typename E>
    Result<T, E> Error(E&& error)
    {
        return Result<T, E>(std::move(error));
    }
}