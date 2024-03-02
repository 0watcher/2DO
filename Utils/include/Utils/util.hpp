#pragma once

#include <any>
#include <chrono>
#include <functional>
#include <optional>
#include <sstream>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#else
#include <cstdlib>
#endif

#include <Utils/type.hpp>

namespace sch = std::chrono;

using TimePoint = sch::time_point<sch::system_clock, sch::minutes>;

using NanoSeconds = sch::nanoseconds;

namespace twodoutils {
[[nodiscard]] NanoSeconds speed_test(std::function<void()> test);

void log_to_file(StringView msg, const String& filepath);

void create_simple_app_env(const String& folder_name,
                           const Vector<String>& files);

void wipe_simple_app_env(const String& folder_name);

[[nodiscard]] String hash(const String& str);

[[nodiscard]] String tptos(const TimePoint&);

[[nodiscard]] TimePoint stotp(const String&);

inline void clear_term() {
#ifdef _WIN32
    ::system("cls");
#else
    ::system("clear");
#endif
}

inline void sleep(const unsigned int time_ms) noexcept {
    std::this_thread::sleep_for(std::chrono::milliseconds(time_ms));
}

template <typename T>
[[nodiscard]] typename std::enable_if<std::is_same<T, String>::value ||
                                          std::is_same<T, TimePoint>::value,
                                      T>::type
get_current_timestamp(const unsigned int additional_days = 0) {
    if constexpr (std::is_same<T, TimePoint>::value) {
        return sch::time_point_cast<sch::minutes>(sch::system_clock::now() +
                                                  sch::days{additional_days});
    } else if constexpr (std::is_same<T, String>::value) {
        sch::system_clock::time_point timestamp =
            sch::system_clock::now() + sch::days{additional_days};

        std::time_t currentTime = sch::system_clock::to_time_t(timestamp);

        std::stringstream ss;
        ss << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S");

        return ss.str();
    }
}

class IUserInputHandler {
  public:
    virtual String get_input() const = 0;
    virtual String get_secret() const = 0;
    virtual ~IUserInputHandler(){};
};

class IPrinter {
  public:
    virtual void msg_print(StringView msg) const = 0;
    virtual void err_print(StringView msg) const = 0;
    virtual void menu_print(StringView page_name,const Vector<StringView>& menu_pages) const = 0;
    virtual ~IPrinter(){};
};
}  // namespace twodoutils

class AssertFail : public std::runtime_error {
  public:
    AssertFail(const char* file, int line, const String& message)
        : std::runtime_error{message}, file{file}, line{line} {}

    const char* get_file() const { return file; }

    int get_line() const { return line; }

    const char* what() const noexcept override {
        std::ostringstream oss;
        oss << "Assertion failed in file " << file << " at line " << line
            << "\nError: " << std::runtime_error::what();
        return std::move(oss.str().c_str());
    }

  private:
    const char* file;
    const int line;
};

#ifndef NDEBUG
#define TDASSERT(expr, msg) \
    if (!(expr))            \
        throw AssertFail(__FILE__, __LINE__, (msg));
#else
#define TDASSERT(expr, msg)
#endif