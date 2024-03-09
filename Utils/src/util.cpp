#include "Utils/util.hpp"

#include <filesystem>
#include <format>
#include <fstream>
#include <mutex>
#include <optional>
#include <sstream>
#include <thread>

namespace fs = std::filesystem;

namespace twodoutils {
NanoSeconds speed_test(const std::function<void()>& test) {
    auto start = std::chrono::high_resolution_clock::now();

    test();

    auto end = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<NanoSeconds>(end - start);
}

void log_to_file(StringView msg, const String& filepath) {
    std::ofstream file{filepath, std::ios_base::ate};
    file << "[" << to_string(get_current_timestamp()) << "] " << msg << '\n';
    file.close();
}

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

String hash(const String& str) {
    unsigned int magic = 7654321;
    unsigned int hash = 123456789;

    for (int i = 0; i < str.length(); ++i) {
        hash = hash ^ (str[i]);
        hash = hash * magic;
    }

    std::stringstream hex_stream;
    hex_stream << std::hex << hash;
    String str_hash = hex_stream.str();

    return str_hash;
}

TimePoint get_current_timestamp(const unsigned int additional_days) {
    return sch::time_point_cast<sch::minutes>(sch::system_clock::now() +
                                              sch::days{additional_days});
}

String to_string(const TimePoint tp) {
    return std::format("{:%Y-%m-%d %H:%M}", tp);
}

std::optional<TimePoint> to_time_point(const String& tp_str) {
    std::istringstream iss(tp_str);
    sch::sys_time<sch::minutes> tp;
    if (!(iss >> std::chrono::parse("%Y-%m-%d %H:%M", tp)).fail())
        return sch::time_point_cast<sch::minutes>(tp);
    else
        return std::nullopt;
}
}  // namespace twodoutils