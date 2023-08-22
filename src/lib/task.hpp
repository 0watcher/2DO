#pragma once

#include <chrono>
#include <cstdint>
#include <map>
#include <string>

#include "result.hpp"

namespace twodo
{
using TimePoint = std::chrono::system_clock::time_point;

class Task
{
   public:
    Task(const int&, const std::string&, const std::string&, const TimePoint&, const bool&);
    Task(const int&, const bool&);

    int get_id() const { return m_id; }
    std::string get_topic() const { return m_topic; }
    std::string get_content() const { return m_content; }
    TimePoint get_deadline() const { return m_deadline; }
    int get_executor_id() const { return m_executor_id; }
    int get_owner_id() const { return m_owner_id; }
    bool get_is_done() const { return m_is_done; }

    void set_topic(const std::string& topic) { m_topic = topic; }
    void set_content(const std::string& content) { m_content = content; }
    void set_executor(const int& id) { m_executor_id = id; }
    void set_owner(const int& id) { m_owner_id = id; }
    void set_is_done(const bool& done) { m_is_done = done; }

   private:
    std::uint16_t m_id {};
    std::string m_topic {};
    std::string m_content {};
    TimePoint m_start_date {};
    TimePoint m_deadline {};
    int m_executor_id {};
    int m_owner_id {};
    std::map<std::string, std::string> discussion {};
    bool m_is_done {};
};
}  // namespace twodo