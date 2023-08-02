#pragma once

#include <memory>
#include <sqlite3.h>
#include <map>
#include <string>

#include "result.hpp"

namespace twodo
{
    struct sql3_closer
    {
        void operator()(sqlite3* p) const
        {
            sqlite3_close_v2(p);
        }
    };

    using sql3_ptr = std::unique_ptr<sqlite3, sql3_closer>;

    class Database
    {
    public:
        Database() = default;

        Result<void> create_or_open(const std::string& path);
        Result<std::string> create_table(const std::string& table_name,
                                         const std::map<std::string, std::string>& column_names);
        Result<std::string> insert_data(const std::string& table_name,
                                        const std::map<std::string, std::string>& values);
        Result<std::string> select_data(const std::string& table_name,
                                        const std::pair<std::string, std::string>& where);
        Result<std::string> update_data(const std::string& table_name, 
                                        const std::pair<std::string, std::string>& set, 
                                        const std::pair<std::string, std::string>& where);

        private:
            sql3_ptr m_db{};
    };
}