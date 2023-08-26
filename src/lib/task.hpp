#pragma once

#include <chrono>
#include <cstdint>
#include <map>
#include <string>

#include "result.hpp"

using stringmap = std::map<std::string, std::string>;
using String = std::string;

namespace twodo
{
using TimePoint = std::chrono::system_clock::time_point;

class Task
{
   public:
    Task(const int&, const String&, const String&, const TimePoint&, const bool&);
    Task(const int&, const bool&);

    int get_id() const { return m_id; }
    String get_topic() const { return m_topic; }
    String get_content() const { return m_content; }
    TimePoint get_deadline() const { return m_deadline; }
    std::uint16_t get_executor_id() const { return m_executor_id; }
    std::uint16_t get_owner_id() const { return m_owner_id; }
    bool get_is_done() const { return m_is_done; }

    void set_topic(const String& topic) { m_topic = topic; }
    void set_content(const String& content) { m_content = content; }
    void set_executor(const std::uint16_t& id) { m_executor_id = id; }
    void set_owner(const std::uint16_t& id) { m_owner_id = id; }
    void set_is_done(const bool& done) { m_is_done = done; }

   private:
    std::uint16_t m_id {};
    String m_topic {};
    String m_content {};
    TimePoint m_start_date {};
    TimePoint m_deadline {};
    std::uint16_t m_executor_id {};
    std::uint16_t m_owner_id {};
    stringmap m_discussion {};
    bool m_is_done {};
};
}  // namespace twodo