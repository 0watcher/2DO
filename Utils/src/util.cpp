#include "Utils/util.hpp"

#include <filesystem>
#include <format>
#include <fstream>
#include <sstream>

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
    file << "[" << format_datetime(get_current_timestamp()) << "] " << msg
         << '\n';
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

String tptos(const TimePoint tp) {
    return std::to_string(
        std::chrono::duration_cast<std::chrono::minutes>(tp.time_since_epoch())
            .count());
}

TimePoint stotp(const String& tp_str) {
    return std::chrono::time_point<std::chrono::system_clock,
                                   std::chrono::minutes>(
        std::chrono::minutes(std::stoll(tp_str)));
}

String format_datetime(const TimePoint tp) {
    return std::format("{:%Y.%m.%d %H:%M}", tp);
}

std::optional<TimePoint> parse_datetime(const String& datetime_str) {
    std::istringstream iss{datetime_str};

    unsigned int year, month, day, hours, minutes;
    char colon, dot, space;

    iss >> year >> dot >> month >> dot >> day >> space >> hours >> colon >>
        minutes;

    const auto date_evaluation = [year, month, day, hours, minutes] {
        return year >= 1970 && month >= 1 && month <= 12 && day >= 1 &&
               day <= 31 && hours >= 0 && hours <= 23 && minutes >= 0 &&
               minutes <= 59;
    };

    const auto chars_evaluation = [colon, dot, space] {
        return colon == ':' && dot == '.' && space == '-';
    };

    if (!date_evaluation() || !chars_evaluation() || iss.fail() || iss.bad()) {
        return std::nullopt;
    }

    return TimePoint{sch::minutes(minutes - 24) +
                     sch::duration_cast<sch::minutes>(sch::hours(hours - 5)) +
                     sch::duration_cast<sch::minutes>(sch::days(day - 1)) +
                     sch::duration_cast<sch::minutes>(sch::months(month - 1)) +
                     sch::duration_cast<sch::minutes>(sch::years(year - 1970))};
}
}  // namespace twodoutils