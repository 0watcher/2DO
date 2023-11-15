#pragma once

#include <chrono>
#include <map>
#include <string>
#include <string_view>

#include "result.hpp"
#include "utils.hpp"

enum class TaskErr
{
    GetTaskFailure,
    AddTaskFailure,
    DeleteTaskFailure,
    UpdateTaskFailure
};

namespace twodo
{
struct Discussion
{
  String d = "";
  bool operator==(const Discussion& other) const
  {
    return d == other.d;
  }
};

class Task
{
   public:
    Task(int id, std::string_view topic, std::string_view content, const TimePoint& start_date,
         const TimePoint& deadline, int eid, int oid, const Discussion& discus, bool is_done)
        : m_id {id},
          m_topic {topic},
          m_content {content},
          m_start_date {start_date},
          m_deadline {deadline},
          m_executor_id {eid},
          m_owner_id {oid},
          m_discussion {discus},
          m_is_done {is_done}
    {
    }

    Task(std::string_view topic, std::string_view content, const TimePoint& start_date,
         const TimePoint& deadline, int eid, int oid, const Discussion& discus, bool is_done)
        : m_topic {topic},
          m_content {content},
          m_start_date {start_date},
          m_deadline {deadline},
          m_executor_id {eid},
          m_owner_id {oid},
          m_discussion {discus},
          m_is_done {is_done}
    {
    }

    bool operator==(const Task& other) const
    {
        return m_id == other.m_id && m_topic == other.m_topic && m_content == other.m_content &&
               m_start_date == other.m_start_date && m_deadline == other.m_deadline &&
               m_executor_id == other.m_executor_id && m_owner_id == other.m_owner_id &&
               m_discussion == other.m_discussion && m_is_done == other.m_is_done;
    }

    int get_id() const { return m_id.value(); }
    String get_topic() const { return m_topic; }
    String get_content() const { return m_content; }
    TimePoint get_start_date() const { return m_start_date; }
    TimePoint get_deadline() const { return m_deadline; }
    int get_executor_id() const { return m_executor_id; }
    int get_owner_id() const { return m_owner_id; }
    Discussion get_discussion() const { return m_discussion; }
    bool get_is_done() const { return m_is_done; }

    void set_id(int id) { m_id = id; };
    void set_topic(std::string_view topic) { m_topic = topic; }
    void set_content(std::string_view content) { m_content = content; }
    void set_executor(int id) { m_executor_id = id; }
    void set_owner(int id) { m_owner_id = id; }
    void set_is_done(bool done) { m_is_done = done; }

   private:
    std::optional<int> m_id {std::nullopt};
    String m_topic {};
    String m_content {};
    TimePoint m_start_date {};
    TimePoint m_deadline {};
    int m_executor_id {};
    int m_owner_id {};
    Discussion m_discussion {};
    bool m_is_done {};
};

class TaskDb
{
   public:
    TaskDb(const String& path);

    [[nodiscard]] Result<Task, TaskErr> get_task(const String& topic);
    [[nodiscard]] Result<Task, TaskErr> get_task(int id);
    [[nodiscard]] Result<Id, TaskErr> get_task_id(const String& topic);
    Result<None, TaskErr> add_task(Task& task);
    Result<None, TaskErr> delete_task(int id);
    Result<None, TaskErr> update_data(const Task& task);

   private:
    Database m_db;
};

}  // namespace twodo