#pragma once

#include <any>
#include <chrono>
#include <functional>
#include <iomanip>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#ifdef _WIN32
#include <windows.h>
#else
#include <cstdlib>
#endif

#include "Utils/type.hpp"

using TimePoint =
    std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes>;

using NanoSeconds = std::chrono::nanoseconds;

class AssertFail : std::runtime_error {
  public:
    AssertFail(const char* file, int line, const String& message)
        : std::runtime_error{message}, file{file}, line{line} {}

    const char* get_file() const { return file; }

    int get_line() { return line; }

    const char* what() const noexcept override {
        std::ostringstream oss;
        oss << "Assertion failed in file " << file << "at line " << line
            << "\nError: " << std::runtime_error::what();
        return std::move(oss.str().c_str());
    }

  private:
    const char* file;
    int line;
};

#ifndef NDEBUG
#define ASSERT(expr, msg) \
    if (!expr)            \
        throw AssertFail(__FILE__, __LINE__, msg);
#else
#endif

namespace twodoutils {
NanoSeconds speed_test(std::function<void()> test);

void log_to_file(StringView msg, const String& filepath);

template <typename T>
[[nodiscard]] typename std::enable_if<std::is_same<T, String>::value ||
                                          std::is_same<T, TimePoint>::value,
                                      T>::type
get_current_timestamp(unsigned int additional_days = 0) {
    if constexpr (std::is_same<T, TimePoint>::value) {
        return std::chrono::time_point_cast<std::chrono::minutes>(
            std::chrono::system_clock::now() +
            std::chrono::days{additional_days});
    } else if constexpr (std::is_same<T, String>::value) {
        std::chrono::system_clock::time_point timestamp =
            std::chrono::system_clock::now() +
            std::chrono::days{additional_days};

        std::time_t currentTime =
            std::chrono::system_clock::to_time_t(timestamp);

        std::stringstream ss;
        ss << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S");

        return ss.str();
    }
}

inline void clear_term() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

std::optional<String> get_base_directory();

void create_simple_app_env(const String& folder_name,
                           const Vector<String>& files);

void wipe_simple_app_env(const String& folder_name);

[[nodiscard]] String hash(const String& str);

void sleep(unsigned int t) noexcept;

[[nodiscard]] String tptos(const TimePoint&) noexcept;
[[nodiscard]] TimePoint stotp(const String&) noexcept;

class IUserInputHandler {
  public:
    virtual String get_input() const = 0;
    virtual ~IUserInputHandler(){};
};

class IPrinter {
  public:
    virtual void msg_print(StringView msg) const = 0;
    virtual void err_print(StringView err) const = 0;
    virtual ~IPrinter(){};
};

class Resource {
  public:
    Resource() = default;
    Resource(const Resource&) = delete;
    Resource& operator=(const Resource&) = delete;
    Resource(Resource&&) = default;
    Resource& operator=(Resource&&) = default;

    void push(std::any&& data) {
        m_resource = std::make_unique<std::any>(std::move(data));
    }

    template <typename ResourceT>
    std::optional<ResourceT> pop() {
        if (m_resource->has_value()) {
            return std::move(*std::any_cast<ResourceT>(m_resource.release()));
        }
        return std::nullopt;
    }

  private:
    std::unique_ptr<std::any> m_resource = nullptr;
};

}  // namespace twodoutils