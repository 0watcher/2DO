#pragma once

#include <SQLiteCpp/Database.h>

#include <Utils/result.hpp>
#include <Utils/type.hpp>
#include <Utils/util.hpp>

namespace tdu = twodoutils;
namespace SQL = SQLite;

namespace twodocore {
class [[nodiscard]] Task {
  public:
    Task(const Task&) = default;
    Task& operator=(const Task&) = default;
    Task(Task&& other) = default;
    Task& operator=(Task&& other) = default;

    Task(unsigned int id,
         const String& topic,
         const String& content,
         const TimePoint& start_date,
         const TimePoint& deadline,
         unsigned int executor_id,
         unsigned int owner_id,
         bool is_done)
        : m_id{id},
          m_topic{topic},
          m_content{content},
          m_start_date{start_date},
          m_deadline{deadline},
          m_executor_id{executor_id},
          m_owner_id{owner_id},
          m_is_done{is_done} {}

    Task(unsigned int id,
         const String& topic,
         const String& content,
         const String& start_date,
         const String& deadline,
         unsigned int executor_id,
         unsigned int owner_id,
         unsigned int is_done)
        : m_id{id},
          m_topic{topic},
          m_content{content},
          m_start_date{tdu::stotp(start_date)},
          m_deadline{tdu::stotp(deadline)},
          m_executor_id{executor_id},
          m_owner_id{owner_id},
          m_is_done{static_cast<bool>(is_done)} {}

    Task(const String& topic,
         const String& content,
         const TimePoint& start_date,
         const TimePoint& deadline,
         unsigned int executor_id,
         unsigned int owner_id,
         bool is_done)
        : m_topic{topic},
          m_content{content},
          m_start_date{start_date},
          m_deadline{deadline},
          m_executor_id{executor_id},
          m_owner_id{owner_id},
          m_is_done{is_done} {}

    bool operator==(const Task& other) const {
        return m_id == other.m_id && m_topic == other.m_topic &&
               m_content == other.m_content &&
               m_start_date == other.m_start_date &&
               m_deadline == other.m_deadline &&
               m_executor_id == other.m_executor_id &&
               m_owner_id == other.m_owner_id && m_is_done == other.m_is_done;
    }

    [[nodiscard]] unsigned int id() const { return m_id; }
    [[nodiscard]] String topic() const { return m_topic; }
    [[nodiscard]] String content() const { return m_content; }
    [[nodiscard]] unsigned int executor_id() const { return m_executor_id; }
    [[nodiscard]] unsigned int owner_id() const { return m_owner_id; }
    [[nodiscard]] bool is_done() const { return m_is_done; }

    template <typename T>
    [[nodiscard]] typename std::enable_if<std::is_same<T, String>::value ||
                                              std::is_same<T, TimePoint>::value,
                                          T>::type
    start_date() const {
        if constexpr (std::is_same<T, String>::value) {
            return tdu::tptos(m_start_date);
        } else if constexpr (std::is_same<T, TimePoint>::value) {
            return m_start_date;
        }
    }

    template <typename T>
    [[nodiscard]] typename std::enable_if<std::is_same<T, String>::value ||
                                              std::is_same<T, TimePoint>::value,
                                          T>::type
    deadline() const {
        if constexpr (std::is_same<T, String>::value) {
            return tdu::tptos(m_deadline);
        } else if constexpr (std::is_same<T, TimePoint>::value) {
            return m_deadline;
        }
    }

    void set_id(unsigned int id) { m_id = id; };
    void set_topic(StringView topic) { m_topic = topic; }
    void set_content(StringView content) { m_content = content; }
    void set_start_date(TimePoint date) { m_start_date = date; }
    void set_deadline(TimePoint date) { m_deadline = date; }
    void set_executor(unsigned int id) { m_executor_id = id; }
    void set_owner(unsigned int id) { m_owner_id = id; }
    void set_is_done(bool done) { m_is_done = done; }

  private:
    unsigned int m_id{};
    String m_topic{};
    String m_content{};
    TimePoint m_start_date{};
    TimePoint m_deadline{};
    unsigned int m_executor_id{};
    unsigned int m_owner_id{};
    bool m_is_done = false;
};

class [[nodiscard]] TaskDb {
  public:
    enum class IdType { Owner, Executor };

    TaskDb(const TaskDb&) = delete;
    TaskDb& operator=(const TaskDb&) = delete;
    TaskDb(TaskDb&& other) = default;
    TaskDb& operator=(TaskDb&& other) = default;

    TaskDb(StringView db_filepath);

    [[nodiscard]] Task get_object(unsigned int id) const;

    [[nodiscard]] bool is_table_empty() const;

    void add_object(Task& task) const;
    void add_object(const Task& task) const;

    void update_object(const Task& task) const;

    void delete_object(unsigned int id) const;

    template <IdType T>
    [[nodiscard]] Vector<Task> get_all_objects(unsigned int id) const {
        SQL::Statement query{m_db, ""};

        if constexpr (T == IdType::Executor) {
            query = SQL::Statement{m_db,
                                   "SELECT * FROM tasks WHERE executor_id = ?"};
        } else {
            query =
                SQL::Statement{m_db, "SELECT * FROM tasks WHERE owner_id = ?"};
        }

        query.bind(1, id);

        Vector<Task> tasks;
        while (query.executeStep()) {
            tasks.push_back(Task{
                (unsigned)query.getColumn(0).getInt(),
                query.getColumn(1).getString(), query.getColumn(2).getString(),
                query.getColumn(3).getString(), query.getColumn(4).getString(),
                (unsigned)query.getColumn(5).getInt(),
                (unsigned)query.getColumn(6).getInt(),
                (unsigned)query.getColumn(7).getInt()});
        }

        return tasks;
    }

  private:
    SQL::Database m_db;
};

class [[nodiscard]] Message {
  public:
    Message(const Message&) = default;
    Message& operator=(const Message&) = default;
    Message(Message&& other) = default;
    Message& operator=(Message&& other) = default;

    Message(unsigned int message_id,
            unsigned int task_id,
            StringView sender_name,
            StringView content,
            TimePoint timestamp)
        : m_message_id{message_id},
          m_task_id{task_id},
          m_sender_name{sender_name},
          m_content{content},
          m_timestamp{timestamp} {}

    Message(unsigned int task_id,
            StringView sender_name,
            StringView content,
            TimePoint timestamp)
        : m_task_id{task_id},
          m_sender_name{sender_name},
          m_content{content},
          m_timestamp{timestamp} {}

    Message(unsigned int task_id,
            StringView sender_name,
            StringView content,
            const String& timestamp)
        : m_task_id{task_id},
          m_sender_name{sender_name},
          m_content{content},
          m_timestamp{tdu::stotp(timestamp)} {}

    bool operator==(const Message& other) const {
        return m_message_id == other.m_message_id &&
               m_task_id == other.m_task_id && m_content == other.m_content &&
               m_sender_name == other.m_sender_name &&
               m_timestamp == other.m_timestamp;
    }

    [[nodiscard]] int message_id() const { return m_message_id; }
    [[nodiscard]] int task_id() const { return m_task_id; }
    [[nodiscard]] String sender_name() const { return m_sender_name; }
    [[nodiscard]] String content() const { return m_content; }

    template <typename T>
    [[nodiscard]] typename std::enable_if<std::is_same<T, String>::value ||
                                              std::is_same<T, TimePoint>::value,
                                          T>::type
    timestamp() const {
        if constexpr (std::is_same<T, String>::value) {
            return tdu::tptos(m_timestamp);
        } else if constexpr (std::is_same<T, TimePoint>::value) {
            return m_timestamp;
        }
    }

    void set_message_id(unsigned int id) { m_message_id = id; }

  private:
    unsigned int m_message_id;
    unsigned int m_task_id;
    String m_sender_name;
    String m_content;
    TimePoint m_timestamp;
};

class [[nodiscard]] MessageDb {
  public:
    MessageDb(const MessageDb&) = delete;
    MessageDb& operator=(const MessageDb&) = delete;
    MessageDb(MessageDb&& other) = default;
    MessageDb& operator=(MessageDb&& other) = default;

    MessageDb(StringView db_filepath);

    [[nodiscard]] Vector<Message> get_all_objects(unsigned int task_id) const;

    [[nodiscard]] bool is_table_empty() const;

    void add_object(Message& message) const;
    void add_object(const Message& message) const;

    void delete_all_by_task_id(unsigned int task_id) const;

  private:
    SQL::Database m_db;
};
}  // namespace twodocore