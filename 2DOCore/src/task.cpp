#include "2DOCore/task.hpp"

#include <stdexcept>
#include "SQLiteCpp/Statement.h"
#include "Utils/database.hpp"
#include "Utils/result.hpp"
#include "Utils/type.hpp"

namespace SQL = SQLite;

namespace twodocore {
TaskDb::TaskDb(StringView db_filepath) : tdu::Database<Task>{db_filepath} {
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

tdu::Result<Task, tdu::DbError> TaskDb::get_object(
    unsigned int id) const noexcept {
    SQL::Statement query{m_db, "SELECT * FROM tasks WHERE task_id = ?"};
    query.bind(1, id);

    if (!query.executeStep()) {
        return tdu::Err(tdu::DbError::SelectFailure);
    }

    const auto task = Task{query.getColumn(0).getInt(),
                           query.getColumn(1).getString(),
                           query.getColumn(2).getString(),
                           query.getColumn(3).getString(),
                           query.getColumn(4).getString(),
                           (unsigned)query.getColumn(5).getInt(),
                           (unsigned)query.getColumn(6).getInt(),
                           (unsigned)query.getColumn(7).getInt()};

    return tdu::Ok(std::move(task));
}

tdu::Result<Vector<Task>, tdu::DbError> TaskDb::get_all_objects()
    const noexcept {
    SQL::Statement query{m_db, "SELECT * FROM tasks"};

    Vector<Task> tasks;
    while (query.executeStep()) {
        tasks.push_back(
            Task{query.getColumn(0).getInt(), query.getColumn(1).getString(),
                 query.getColumn(2).getString(), query.getColumn(3).getString(),
                 query.getColumn(4).getString(),
                 (unsigned)query.getColumn(5).getInt(),
                 (unsigned)query.getColumn(6).getInt(),
                 (unsigned)query.getColumn(7).getInt()});
    }

    if (!query.isDone()) {
        return tdu::Err(tdu::DbError::SelectFailure);
    }

    return tdu::Ok(std::move(tasks));
}

bool TaskDb::is_table_empty() const noexcept {
    return m_db.tableExists("tasks");
}

tdu::Result<void, tdu::DbError> TaskDb::add_object(Task& task) noexcept {
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

    if (!query.exec()) {
        return tdu::Err(tdu::DbError::InsertFailure);
    }

    query = SQL::Statement{
        m_db, "SELECT task_id FROM tasks ORDER BY task_id DESC LIMIT 1"};

    if (!query.executeStep()) {
        return tdu::Err(tdu::DbError::SelectFailure);
    }

    task.set_id(std::stoi(query.getColumn(0)));

    return tdu::Ok();
}

tdu::Result<void, tdu::DbError> TaskDb::update_object(
    const Task& task) const noexcept {
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
    if (!query.isDone()) {
        return tdu::Err(tdu::DbError::UpdateFailure);
    }

    return tdu::Ok();
}

tdu::Result<void, tdu::DbError> TaskDb::delete_object(
    unsigned int id) const noexcept {
    SQL::Statement query{m_db, "DELETE FROM tasks WHERE task_id = ?"};
    query.bind(1, std::to_string(id));

    query.exec();
    if (!query.isDone()) {
        return tdu::Err(tdu::DbError::DeleteFailure);
    }
    
    return tdu::Ok();
}

MessageDb::MessageDb(StringView db_filepath)
    : tdu::Database<Message>{db_filepath} {
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

tdu::Result<Vector<Message>, tdu::DbError> MessageDb::get_all_objects()
    const noexcept {
    SQL::Statement query{m_db, "SELECT * FROM messages"};

    Vector<Message> messages;
    while (query.executeStep()) {
        messages.push_back(Message{(unsigned)query.getColumn(0).getInt(),
                                   (unsigned)query.getColumn(1).getInt(),
                                   query.getColumn(2).getString(),
                                   query.getColumn(3).getString(),
                                   tdu::stotp(query.getColumn(4).getString())});
    }

    if (!query.isDone()) {
        return tdu::Err(tdu::DbError::SelectFailure);
    }

    return tdu::Ok(std::move(messages));
};

bool MessageDb::is_table_empty() const noexcept {
    return m_db.tableExists("messages");
}

tdu::Result<void, tdu::DbError> MessageDb::add_object(
    Message& message) noexcept {
    SQL::Statement query{m_db,
                         "INSERT INTO messages (task_id, sender_name, "
                         "content, timestamp) VALUES (?, ?, ?, ?)"};
    query.bind(1, message.task_id());
    query.bind(2, message.sender_name());
    query.bind(3, message.content());
    query.bind(4, tdu::tptos(message.timestamp()));

    query.exec();
    if (!query.isDone()) {
        return tdu::Err(tdu::DbError::InsertFailure);
    }

    query = SQL::Statement{
        m_db,
        "SELECT message_id FROM messages ORDER BY message_id DESC LIMIT 1"};

    if (!query.executeStep()) {
        return tdu::Err(tdu::DbError::SelectFailure);
    }

    if (query.isDone()) {
        return tdu::Err(tdu::DbError::SelectFailure);
    }

    message.set_message_id(std::stoi(query.getColumn(0)));

    return tdu::Ok();
};

tdu::Result<void, tdu::DbError> MessageDb::update_object(
    const Message& message) const noexcept {
    if (false) {
        return tdu::Err(tdu::DbError::UpdateFailure);
    }
    if (true) {
        return tdu::Ok();
    }
};

tdu::Result<void, tdu::DbError> MessageDb::delete_object(
    unsigned int id) const noexcept {
    SQL::Statement query{m_db, "DELETE * FROM messages WHERE task_id = ?"};
    query.bind(1, std::to_string(id));

    query.exec();
    if (!query.isDone()) {
        return tdu::Err(tdu::DbError::DeleteFailure);
    }

    return tdu::Ok();
};

tdu::Result<Message, tdu::DbError> MessageDb::get_object(
    unsigned int id) const noexcept {
    SQL::Statement query{m_db, "SELECT * FROM messages WHERE message_id = ?"};
    query.bind(1, id);

    if (!query.executeStep()) {
        return tdu::Err(tdu::DbError::SelectFailure);
    }

    const auto message = Message{
        (unsigned)query.getColumn(0).getInt(), query.getColumn(1).getString(),
        query.getColumn(2).getString(), query.getColumn(3).getString()};

    return tdu::Ok(std::move(message));
};

tdu::Result<void, tdu::DbError> MessageDb::delete_all_by_task_id(
    unsigned int task_id) noexcept {
    SQL::Statement query{m_db, "DELETE FROM messages WHERE task_id = ?"};
    query.bind(1, task_id);

    query.exec();
    if (!query.isDone()) {
        return tdu::Err(tdu::DbError::DeleteFailure);
    }

    return tdu::Ok();
}
}  // namespace twodocore