#include "2DOCore/task.hpp"

#include <SQLiteCpp/Statement.h>
#include <optional>
#include "Utils/util.hpp"

namespace twodocore {
TaskDb::TaskDb(StringView db_filepath)
    : m_db{db_filepath, SQL::OPEN_READWRITE | SQL::OPEN_CREATE} {
    if (!is_table_empty()) {
        SQL::Statement query{
            m_db,
            "CREATE TABLE IF NOT EXISTS tasks ("
            "task_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
            "topic VARCHAR(20) NOT NULL, "
            "content TEXT NOT NULL, "
            "start_date VARCHAR(10) NOT NULL, "
            "deadline VARCHAR(10) NOT NULL, "
            "executor_id INTEGER NOT NULL, "
            "owner_id INTEGER NOT NULL, "
            "is_done BOOLEAN NOT NULL)"};

        query.exec();
        if (!query.isDone()) {
            throw std::runtime_error("Failure creating tasks table.");
        }
    }
}

Task TaskDb::get_object(const unsigned int id) const {
    SQL::Statement query{m_db, "SELECT * FROM tasks WHERE task_id = ?"};
    query.bind(1, id);

    query.executeStep();

    return Task{(unsigned)query.getColumn(0).getInt(),
                query.getColumn(1).getString(),
                query.getColumn(2).getString(),
                query.getColumn(3).getString(),
                query.getColumn(4).getString(),
                (unsigned)query.getColumn(5).getInt(),
                (unsigned)query.getColumn(6).getInt(),
                (unsigned)query.getColumn(7).getInt()};
}

bool TaskDb::is_table_empty() const {
    return m_db.tableExists("tasks");
}

void TaskDb::add_object(Task& task) const {
    SQL::Statement query{
        m_db,
        "INSERT INTO tasks (topic, content, start_date, deadline, "
        "executor_id, owner_id, is_done) VALUES (?, ?, ?, ?, "
        "?, ?, ?)"};
    query.bind(1, task.topic());
    query.bind(2, task.content());
    query.bind(3, task.start_date<String>());
    query.bind(4, task.deadline<String>());
    query.bind(5, task.executor_id());
    query.bind(6, task.owner_id());
    query.bind(7, task.is_done());

    query.exec();

    query = SQL::Statement{
        m_db, "SELECT task_id FROM tasks ORDER BY task_id DESC LIMIT 1"};

    query.executeStep();

    task.set_id(std::stoi(query.getColumn(0)));
}

void TaskDb::add_object(const Task& task) const {
    SQL::Statement query{
        m_db,
        "INSERT INTO tasks (topic, content, start_date, deadline, "
        "executor_id, owner_id, is_done) VALUES (?, ?, ?, ?, "
        "?, ?, ?)"};
    query.bind(1, task.topic());
    query.bind(2, task.content());
    query.bind(3, task.start_date<String>());
    query.bind(4, task.deadline<String>());
    query.bind(5, task.executor_id());
    query.bind(6, task.owner_id());
    query.bind(7, task.is_done());

    query.exec();

    query = SQL::Statement{
        m_db, "SELECT task_id FROM tasks ORDER BY task_id DESC LIMIT 1"};

    query.executeStep();
}

void TaskDb::update_object(const Task& task) const {
    SQL::Statement query{
        m_db,
        "UPDATE tasks SET topic = ?, content = ?, start_date = ?, deadline "
        "= ?, executor_id = ?, owner_id = ?, is_done = ? "
        "WHERE task_id = ?"};
    query.bind(1, task.topic());
    query.bind(2, task.content());
    query.bind(3, task.start_date<String>());
    query.bind(4, task.deadline<String>());
    query.bind(5, task.executor_id());
    query.bind(6, task.owner_id());
    query.bind(7, task.is_done());
    query.bind(8, task.id());

    query.exec();
}

void TaskDb::delete_object(const unsigned int id) const {
    SQL::Statement query{m_db, "DELETE FROM tasks WHERE task_id = ?"};
    query.bind(1, std::to_string(id));

    query.exec();
}

MessageDb::MessageDb(StringView db_filepath)
    : m_db{db_filepath, SQL::OPEN_READWRITE | SQL::OPEN_CREATE} {
    if (!is_table_empty()) {
        SQL::Statement query{
            m_db,
            "CREATE TABLE IF NOT EXISTS messages ("
            "message_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
            "task_id INTEGER NOT NULL, "
            "sender_name VARCHAR(20) NOT NULL, "
            "content VARCHAR(200) NOT NULL, "
            "timestamp VARCHAR(10) NOT NULL)"};

        query.exec();
        if (!query.isDone())
            throw std::runtime_error("Failure creating messages table.");
    }
}

std::optional<Message> MessageDb::get_newest_object() const {
    SQL::Statement query{
        m_db, "SELECT * FROM messages ORDER BY message_id DESC LIMIT 1"};

    try {
        if (!query.executeStep()) {
            return std::nullopt;
        }
    } catch (const std::exception& e) {
        if (query.hasRow()) {
            throw e;
        }
    }

    return Message{(unsigned)query.getColumn(0).getInt(),
                   (unsigned)query.getColumn(1).getInt(),
                   query.getColumn(2).getString(),
                   query.getColumn(3).getString(),
                   tdu::to_time_point(query.getColumn(4).getString()).value()};
}

Vector<Message> MessageDb::get_all_objects(const unsigned int taks_id) const {
    SQL::Statement query{m_db, "SELECT * FROM messages WHERE task_id = ?"};
    query.bind(1, taks_id);

    Vector<Message> messages;
    while (query.executeStep()) {
        messages.push_back(Message{
            (unsigned)query.getColumn(0).getInt(),
            (unsigned)query.getColumn(1).getInt(),
            query.getColumn(2).getString(), query.getColumn(3).getString(),
            tdu::to_time_point(query.getColumn(4).getString()).value()});
    }

    return messages;
};

bool MessageDb::is_table_empty() const {
    return m_db.tableExists("messages");
}

void MessageDb::add_object(Message& message) const {
    SQL::Statement query{m_db,
                         "INSERT INTO messages (task_id, sender_name, "
                         "content, timestamp) VALUES (?, ?, ?, ?)"};
    query.bind(1, message.task_id());
    query.bind(2, message.sender_name());
    query.bind(3, message.content());
    query.bind(4, message.timestamp<String>());

    query.exec();

    query = SQL::Statement{
        m_db,
        "SELECT message_id FROM messages ORDER BY message_id DESC LIMIT 1"};

    query.executeStep();

    message.set_message_id(std::stoi(query.getColumn(0)));
};

void MessageDb::add_object(const Message& message) const {
    SQL::Statement query{m_db,
                         "INSERT INTO messages (task_id, sender_name, "
                         "content, timestamp) VALUES (?, ?, ?, ?)"};
    query.bind(1, message.task_id());
    query.bind(2, message.sender_name());
    query.bind(3, message.content());
    query.bind(4, message.timestamp<String>());

    query.exec();

    query = SQL::Statement{
        m_db,
        "SELECT message_id FROM messages ORDER BY message_id DESC LIMIT 1"};

    query.executeStep();
}

void MessageDb::delete_all_by_task_id(const unsigned int task_id) const {
    SQL::Statement query{m_db, "DELETE FROM messages WHERE task_id = ?"};
    query.bind(1, task_id);

    query.exec();
}
}  // namespace twodocore