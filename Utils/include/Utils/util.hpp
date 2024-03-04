#pragma once

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
[[nodiscard]] NanoSeconds speed_test(const std::function<void()>& test);

void log_to_file(StringView msg, const String& filepath);

void create_simple_app_env(const String& folder_name,
                           const Vector<String>& files);

void wipe_simple_app_env(const String& folder_name);

[[nodiscard]] String hash(const String& str);

[[nodiscard]] TimePoint get_current_timestamp(
    const unsigned int additional_days = 0);

[[nodiscard]] String tptos(const TimePoint tp);

[[nodiscard]] TimePoint stotp(const String& tp_str);

[[nodiscard]] String format_datetime(const TimePoint tp);

[[nodiscard]] std::optional<TimePoint> parse_datetime(
    const String& datetime_str);

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
    virtual void menu_print(
        StringView page_name,
        const HashMap<String, String>& menu_pages) const = 0;
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