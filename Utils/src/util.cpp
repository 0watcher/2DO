#include "Utils/util.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "Utils/type.hpp"

namespace fs = std::filesystem;

namespace twodoutils {
namespace {
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
}  // namespace

void log_to_file(StringView msg, const String& filepath) {
    std::ofstream file{filepath, std::ios_base::ate};
    file << "[" << get_current_timestamp<String>() << "] " << msg << '\n';
    file.close();
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

[[nodiscard]] String tptos(const TimePoint& tp) {
    std::time_t time = std::chrono::system_clock::to_time_t(tp);

    const std::chrono::minutes minutes =
        std::chrono::duration_cast<std::chrono::minutes>(tp.time_since_epoch());

    return std::to_string(minutes.count());
}

[[nodiscard]] TimePoint stotp(const String& stringified_tp) {
    const auto minutes_count = std::stoll(stringified_tp);

    const TimePoint tp = std::chrono::time_point<std::chrono::system_clock,
                                                 std::chrono::minutes>(
        std::chrono::minutes(minutes_count));

    return tp;
}

[[nodiscard]] String hash(const String& str) {
    unsigned int init = 123456789;
    unsigned int magic = 7654321;
    unsigned int hash = init;

    for (int i = 0; i < str.length(); ++i) {
        hash = hash ^ (str[i]);
        hash = hash * magic;
    }

    std::stringstream hex_stream;
    hex_stream << std::hex << hash;
    String str_hash = hex_stream.str();

    return str_hash;
}

NanoSeconds speed_test(std::function<void()> test) {
    auto start = std::chrono::high_resolution_clock::now();

    test();

    auto end = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<NanoSeconds>(end - start);
}
}  // namespace twodoutils