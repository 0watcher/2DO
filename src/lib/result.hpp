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
        const T& value() const noexcept { return std::get<T>(m_storage); }

        const E& error() const noexcept { return std::get<E>(m_storage); }

        explicit operator bool() const noexcept { return !std::holds_alternative<E>(m_storage); }

    private:
        template <typename U, typename V>
        friend Result<U, V> Ok(const U value) noexcept;

        template <typename U, typename V>
        friend Result<U, V> Err(V error) noexcept;

        Result(T&& value) noexcept : m_storage(std::move(value)) {}
        Result(E&& error) noexcept : m_storage(std::move(error)) {}
        Result(None&) {}

        std::variant<T, E> m_storage;
    };

    template <typename T, typename E>
    Result<T, E> Ok(T value) noexcept
    {
        return Result<T, E>(std::move(value));
    }

    template <typename T, typename E>
    Result<T, E> Err(E error) noexcept
    {
        return Result<T, E>(std::move(error));
    }
}  // namespace twodo