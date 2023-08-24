#pragma once

#include <iostream>
#include <variant>

namespace twodo
{
    struct None {};

    template <typename T, typename E>
    class Result
    {
    public:
        const T& value() const { return std::get<T>(m_storage); }

        const E& error() const { return std::get<E>(m_storage); }

        explicit operator bool() const { return !std::holds_alternative<E>(m_storage); }

    private:
        template <typename U, typename V>
        friend Result<U, V> Ok(const U value);

        template <typename U, typename V>
        friend Result<U, V> Err(V error);

        Result(T&& value) : m_storage(std::move(value)) {}
        Result(E&& error) : m_storage(std::move(error)) {}
        Result(None&) {}

        std::variant<T, E> m_storage;
    };

    template <typename T, typename E>
    Result<T, E> Ok(T value)
    {
        return Result<T, E>(std::move(value));
    }

    template <typename T, typename E>
    Result<T, E> Err(E error)
    {
        return Result<T, E>(std::move(error));
    }
}  // namespace twodo