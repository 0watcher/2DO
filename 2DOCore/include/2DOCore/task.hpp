#pragma once

#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <Utils/database.hpp>
#include <Utils/result.hpp>
#include <Utils/type.hpp>
#include <Utils/util.hpp>

namespace tdu = twodoutils;

namespace twodocore {
class [[nodiscard]] Task {
  public:
    Task(const Task&) = default;
    Task& operator=(const Task&) = default;
    Task(Task&& other) = default;
    Task& operator=(Task&& other) = default;

    Task(int id,
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

    Task(int id,
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

    [[nodiscard]] unsigned int id() const { return m_id.value(); }
    [[nodiscard]] String topic() const { return m_topic; }
    [[nodiscard]] String content() const { return m_content; }

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
    [[nodiscard]] unsigned int executor_id() const { return m_executor_id; }
    [[nodiscard]] unsigned int owner_id() const { return m_owner_id; }
    [[nodiscard]] bool is_done() const { return m_is_done; }

    void set_id(unsigned int id) { m_id = id; };
    void set_topic(StringView topic) { m_topic = topic; }
    void set_content(StringView content) { m_content = content; }
    void set_start_date(TimePoint date) { m_start_date = date; }
    void set_deadline(TimePoint date) { m_deadline = date; }
    void set_executor(unsigned int id) { m_executor_id = id; }
    void set_owner(unsigned int id) { m_owner_id = id; }
    void set_is_done(bool done) { m_is_done = done; }

  private:
    std::optional<int> m_id{std::nullopt};
    String m_topic{};
    String m_content{};
    TimePoint m_start_date{};
    TimePoint m_deadline{};
    unsigned int m_executor_id{};
    unsigned int m_owner_id{};
    bool m_is_done = false;
};

class [[nodiscard]] TaskDb : protected tdu::Database<Task> {
  public:
    TaskDb(const TaskDb&) = delete;
    TaskDb& operator=(const TaskDb&) = delete;
    TaskDb(TaskDb&& other) = default;
    TaskDb& operator=(TaskDb&& other) = default;

    TaskDb(StringView db_filepath);

    tdu::Result<Task, tdu::DbError> get_object(
        unsigned int id) const noexcept override;

    tdu::Result<Vector<Task>, tdu::DbError> get_all_objects()
        const noexcept override;

    bool is_table_empty() const noexcept override;

    tdu::Result<void, tdu::DbError> add_object(Task& task) noexcept override;

    tdu::Result<void, tdu::DbError> update_object(
        const Task& task) const noexcept override;

    tdu::Result<void, tdu::DbError> delete_object(
        unsigned int id) const noexcept override;
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
    [[nodiscard]] TimePoint timestamp() const { return m_timestamp; }

    void set_message_id(unsigned int id) { m_message_id = id; }

  private:
    unsigned int m_message_id;
    unsigned int m_task_id;
    String m_sender_name;
    String m_content;
    TimePoint m_timestamp;
};

class [[nodiscard]] MessageDb : protected tdu::Database<Message> {
  public:
    MessageDb(const MessageDb&) = delete;
    MessageDb& operator=(const MessageDb&) = delete;
    MessageDb(MessageDb&& other) = default;
    MessageDb& operator=(MessageDb&& other) = default;

    MessageDb(StringView db_filepath);

    tdu::Result<Vector<Message>, tdu::DbError> get_all_objects()
        const noexcept override;

    bool is_table_empty() const noexcept override;

    tdu::Result<void, tdu::DbError> add_object(
        Message& message) noexcept override;

    tdu::Result<void, tdu::DbError> delete_all_by_task_id(
        unsigned int task_id) noexcept;

    tdu::Result<Message, tdu::DbError> get_object(
        unsigned int id) const noexcept override;

    tdu::Result<void, tdu::DbError> update_object(
        const Message& message) const noexcept override;

    tdu::Result<void, tdu::DbError> delete_object(
        unsigned int id) const noexcept override;
};
}  // namespace twodocore