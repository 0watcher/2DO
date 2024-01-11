#pragma once

#ifdef _WIN32
#include <windows.h>
#else
#include <cstdlib>
#endif

#include <chrono>
#include <optional>

#include "Utils/type.hpp"

using TimePoint =
    std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes>;

namespace twodoutils {
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

[[nodiscard]] String input();

[[nodiscard]] String hash(const String& str);

void sleep(int t) noexcept;

[[nodiscard]] TimePoint give_date(int days) noexcept;

[[nodiscard]] String tptos(const TimePoint&) noexcept;
[[nodiscard]] TimePoint stotp(const String&) noexcept;

template <typename T>
class [[nodiscard]] IUserInputHandler {
  public:
    virtual T get_input() = 0;
    virtual ~IUserInputHandler() = default;
};

class [[nodiscard]] IDisplayer {
  public:
    virtual void msg_display(StringView msg) = 0;
    virtual void err_display(StringView err) = 0;
    virtual ~IDisplayer() = default;
};
}  // namespace twodoutils