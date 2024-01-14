#include "Utils/util.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <thread>

namespace fs = std::filesystem;

namespace twodoutils {
std::optional<String> get_base_directory() {
#ifdef _WIN32
    const auto home = fs::path(getenv("USERPROFILE"));
#else
    const auto home = fs::path(getenv("HOME"));
#endif

    if (!home.empty()) {
        return (home / ".config").string();
    } else {
        return std::nullopt;
    }
}

void create_simple_app_env(const String& folder_name,
                           const Vector<String>& files) {
    const auto base_directory = get_base_directory();

    if (!base_directory) {
        throw std::runtime_error("Unable to determine the base directory.");
    }

    const fs::path folder_path = fs::path(*base_directory) / folder_name;

    try {
        fs::create_directories(folder_path);
    } catch (const std::exception& e) {
        throw std::runtime_error("Unable to create app base folder.");
    }

    for (const auto& file_name : files) {
        const fs::path file_path = folder_path / file_name;
        std::ofstream file{file_path};

        if (!file.is_open()) {
            throw std::runtime_error("Unable to create env app file.");
        }
    }
}

void wipe_simple_app_env(const String& folder_name) {
    const auto base_directory = get_base_directory();

    if (!base_directory) {
        throw std::runtime_error("Unable to determine the base directory.");
    }

    const fs::path folder_path = fs::path(*base_directory) / folder_name;

    try {
        fs::remove_all(folder_path);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failure wiping env app data.");
    }
}

[[nodiscard]] String tptos(const TimePoint& tp) noexcept {
    std::time_t time = std::chrono::system_clock::to_time_t(tp);

    const std::chrono::minutes minutes =
        std::chrono::duration_cast<std::chrono::minutes>(tp.time_since_epoch());

    return std::to_string(minutes.count());
}

[[nodiscard]] TimePoint stotp(const String& stringified_tp) noexcept {
    const auto minutes_count = std::stoll(stringified_tp);

    const TimePoint tp = std::chrono::time_point<std::chrono::system_clock,
                                           std::chrono::minutes>(
        std::chrono::minutes(minutes_count));

    return tp;
}

[[nodiscard]] String input() {
    String input{};
    std::getline(std::cin, input);
    if (std::cin.fail()) {
        throw std::runtime_error("Failure getting value.");
    }
    return input;
}

[[nodiscard]] String hash(const String& str) {
    std::hash<String> hasher{};
    const auto hashed_value = std::to_string(hasher(str));
    if (hashed_value.empty()) {
        throw std::runtime_error("Failure hashing value.");
    }
    return hashed_value;
}

void sleep(int t) noexcept {
    std::this_thread::sleep_for(std::chrono::milliseconds(t));
}

[[nodiscard]] TimePoint give_date(int days) noexcept {
    return std::chrono::time_point_cast<std::chrono::minutes>(
        std::chrono::system_clock::now() + std::chrono::days{days});
}
}  // namespace twodoutils