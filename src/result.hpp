#pragma once

#include <iostream>
#include <string>
#include <stdexcept>
#include <optional>

namespace twodo
{
    enum class ErrorCode
    {
        overall_err = 1,
        incorrect_nickname,
        already_existing_name,
        incorrect_password,
        not_match_regex,
        user_added_successfully
    };

    template <typename T>
    class Result
    {
    public:
        enum class Tag
        {
            ok,
            err
        };

        union Value
        {
            T ok;
            ErrorCode err;

            Value() {}
            ~Value() {}
        };

        Result(const T& data) : tag(Tag::ok)
        {
            new (&value.ok) T(data);
        }

        Result(const ErrorCode& error) : tag(Tag::err)
        {
            new (&value.err) ErrorCode(error);
        }

        ~Result()
        {
            if (tag == Tag::ok)
            {
                value.ok.~T();
            }
            else
            {
                value.err.~ErrorCode();
            }
        }

        Result(Result&& other) : tag(other.tag)
        {
            if (tag == Tag::Ok)
            {
                new (&value.ok) T(std::move(other.value.ok));
            }
            else
            {
                new (&value.err) ErrorCode(std::move(other.value.err));
            }
        }

        bool ok() const
        {
            return tag == Tag::ok;
        }

        bool err() const
        {
            return tag == Tag::err;
        }

        T& unwrap()
        {
            if (tag == Tag::ok)
            {
                return value.ok;
            }
            else
            {
                throw std::runtime_error("Tried to unwrap an error value");
            }
        }

        const T& unwrap() const
        {
            if (tag == Tag::Ok)
            {
                return value.ok;
            }
            else
            {
                throw std::runtime_error("Tried to unwrap an error value");
            }
        }

        ErrorCode unwrap_err()
        {
            if (tag == Tag::err)
            {
                return value.err;
            }
            else
            {
                throw std::runtime_error("Tried to unwrap an ok value");
            }
        }

        const ErrorCode unwrap_err() const
        {
            if (tag == Tag::err)
            {
                return value.err;
            }
            else
            {
                throw std::runtime_error("Tried to unwrap an ok value");
            }
        }

        std::optional<T> expect(const std::string& message)
        {
            if (tag == Tag::ok)
            {
                return value.ok;
            }
            else
            {
                std::cerr << "Error: " << message << std::endl;
                return std::nullopt;
            }
        }

    private:
        Tag tag;
        Value value;
    };

    template <>
    class Result<void>
    {
    public:
        Result(const ErrorCode& error) : tag(Tag::err), value(error) {}

        bool ok() const
        {
            return tag != Tag::err;
        }

        bool err() const
        {
            return tag == Tag::err;
        }

        ErrorCode unwrap_err() const
        {
            return value;
        }

    private:
        enum class Tag
        {
            ok,
            err
        };

        Tag tag;
        ErrorCode value;
    };
}
