#pragma once

#include <Utils/database.hpp>
#include <Utils/result.hpp>
#include <Utils/type.hpp>
#include <Utils/util.hpp>

namespace tdl = twodoutils;

enum class TaskErr {
    GetTaskFailure = 1,
    AddTaskFailure,
    DeleteTaskFailure,
    UpdateTaskFailure,
    AddMessageFailure
};

namespace twodocore {
class Task {
  public:
    Task(const Task&) = default;
    Task& operator=(const Task&) = default;
    Task(Task&& other) = default;
    Task& operator=(Task&& other) = default;

    Task(int id,
         StringView topic,
         StringView content,
         const TimePoint& start_date,
         const TimePoint& deadline,
         int eid,
         int oid,
         int crid,
         bool is_done)
        : m_id{id},
          m_topic{topic},
          m_content{content},
          m_start_date{start_date},
          m_deadline{deadline},
          m_executor_id{eid},
          m_owner_id{oid},
          m_chatroom_id{crid},
          m_is_done{is_done} {}

    Task(StringView topic,
         StringView content,
         const TimePoint& start_date,
         const TimePoint& deadline,
         int eid,
         int oid,
         int crid,
         bool is_done)
        : m_topic{topic},
          m_content{content},
          m_start_date{start_date},
          m_deadline{deadline},
          m_executor_id{eid},
          m_owner_id{oid},
          m_chatroom_id{crid},
          m_is_done{is_done} {}

    bool operator==(const Task& other) const {
        return m_id == other.m_id && m_topic == other.m_topic &&
               m_content == other.m_content &&
               m_start_date == other.m_start_date &&
               m_deadline == other.m_deadline &&
               m_executor_id == other.m_executor_id &&
               m_owner_id == other.m_owner_id &&
               m_chatroom_id == other.m_chatroom_id &&
               m_is_done == other.m_is_done;
    }

    [[nodiscard]] int id() const { return m_id.value(); }
    [[nodiscard]] String topic() const { return m_topic; }
    [[nodiscard]] String content() const { return m_content; }
    [[nodiscard]] TimePoint start_date() const { return m_start_date; }
    [[nodiscard]] TimePoint deadline() const { return m_deadline; }
    [[nodiscard]] int executor_id() const { return m_executor_id; }
    [[nodiscard]] int owner_id() const { return m_owner_id; }
    [[nodiscard]] int chatroom_id() const { return m_chatroom_id; }
    [[nodiscard]] bool is_done() const { return m_is_done; }

    void set_id(int id) { m_id = id; };
    void set_topic(StringView topic) { m_topic = topic; }
    void set_content(StringView content) { m_content = content; }
    void set_start_date(TimePoint date) { m_start_date = date; }
    void set_deadline(TimePoint date) { m_deadline = date; }
    void set_executor(int id) { m_executor_id = id; }
    void set_owner(int id) { m_owner_id = id; }
    void set_is_done(bool done) { m_is_done = done; }

  private:
    std::optional<int> m_id{std::nullopt};
    String m_topic{};
    String m_content{};
    TimePoint m_start_date{};
    TimePoint m_deadline{};
    int m_executor_id{};
    int m_owner_id{};
    int m_chatroom_id{};
    bool m_is_done{};
};

struct Message {
    String sender;
    String content;
    TimePoint timestamp;
    int crid;
};

class TaskDb {
  public:
    TaskDb(const TaskDb&) = delete;
    TaskDb& operator=(const TaskDb&) = delete;
    TaskDb(TaskDb&& other) = default;
    TaskDb& operator=(TaskDb&& other) = default;

    TaskDb(const String& path);

    [[nodiscard]] tdl::Result<Task, TaskErr> get_task(const String& topic);
    [[nodiscard]] tdl::Result<Task, TaskErr> get_task(int id);
    [[nodiscard]] tdl::Result<Id, TaskErr> get_task_id(const String& topic);
    tdl::Result<tdl::None, TaskErr> add_task(Task& task);
    tdl::Result<tdl::None, TaskErr> delete_task(int id);
    tdl::Result<tdl::None, TaskErr> update_data(const Task& task);
    tdl::Result<tdl::None, TaskErr> add_message(Message msg);

  private:
    tdl::Database m_db;
};
}  // namespace twodocore