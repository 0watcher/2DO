#pragma once

#include <SQLiteCpp/SQLiteCpp.h>

#include <string>
#include <vector>

namespace twodo
{
    class DBFileParser
    {
    public:
        DBFileParser() = default;

        std::vector<std::string> read_line(const std::string& file_path, const std::string& key);
        void write_line(const std::string& file_path, const std::string& line);
        void delete_line(const std::string& file_path, const std::string& key);
        void update_line(const std::string& file_path, const std::string& key, const std::string& new_line);
        bool is_key_found(const std::string& file_path, const std::string& key);
    };
}