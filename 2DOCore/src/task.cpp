#include "2DOCore/task.hpp"

#include <stdexcept>
#include "Utils/type.hpp"

namespace twodocore {
TaskDb::TaskDb(StringView db_filepath) {
    m_db = SQL::Database{db_filepath};

    if (!is_table_empty()) {
        SQL::Statement query{
            m_db,
            "CREATE TABLE tasks IF NOT EXISTS (task_id INTEGER "
            "AUTOINCREMENT PRIMARY KEY NOT NULL, topic VARCHAR(20) NOT "
            "NULL, content TEXT NOT NULL, start_date VARCHAR(10) NOT NULL, "
            "deadline VARCHAR(10) NOT NULL, executor_id INTEGER NOT NULL, "
            "owner_id INTEGER NOT NULL, chatoom_id INTEGER NOT NULL, "
            "is_done BOOLEAN NOT NULL)"};

        query.exec();
        if (!query.isDone())
            throw std::runtime_error("Failure creating tasks table.");
    }
}

tdl::Result<Task, tdl::DbError> TaskDb::get_object_by_id(
    int id) const noexcept {
    SQL::Statement query{m_db, "SELECT * FROM tasks WHERE task_id = ?"};
    query.bind(1, id);

    query.exec();
    const auto user =
        Task{query.getColumn(0).getInt(),    query.getColumn(1).getString(),
             query.getColumn(2).getString(), query.getColumn(3).getString(),
             query.getColumn(4).getString(), query.getColumn(5).getInt(),
             query.getColumn(6).getInt(),    query.getColumn(7).getInt(),
             query.getColumn(8).getInt()};

    if (!query.isDone()) {
        return tdl::Err(tdl::DbError::SelectFailure);
    }

    return tdl::Ok(std::move(user));
}

tdl::Result<Vector<Task>, tdl::DbError> TaskDb::get_all_objects()
    const noexcept {
    SQL::Statement query{m_db, "SELECT * FROM tasks"};

    Vector<Task> tasks;
    while (query.executeStep()) {
        tasks.push_back(
            Task{query.getColumn(0).getInt(), query.getColumn(1).getString(),
                 query.getColumn(2).getString(), query.getColumn(3).getString(),
                 query.getColumn(4).getString(), query.getColumn(5).getInt(),
                 query.getColumn(6).getInt(), query.getColumn(7).getInt(),
                 query.getColumn(8).getInt()});
    }

    if (!query.isDone()) {
        return tdl::Err(tdl::DbError::SelectFailure);
    }

    return tdl::Ok(std::move(tasks));
}

bool TaskDb::is_table_empty() const noexcept {
    return m_db.tableExists("tasks");
}

tdl::Result<void, tdl::DbError> TaskDb::add_object(Task& task) noexcept {
    SQL::Statement query{
        m_db,
        "INSERT INTO users (topic, content, start_date, deadline, "
        "executor_id, owner_id, chatroom_id, is_done) VALUES (?, ?, ?, ?, "
        "?, ?, ?, ?)"};
    query.bind(1, task.topic());
    query.bind(2, task.content());
    query.bind(3, task.start_date<String>());
    query.bind(4, task.deadline<String>());
    query.bind(5, task.executor_id());
    query.bind(6, task.owner_id());
    query.bind(7, task.chatroom_id());
    query.bind(8, task.is_done());

    query.exec();
    if (!query.isDone()) {
        return tdl::Err(tdl::DbError::InsertFailure);
    }

    query = SQL::Statement{
        m_db, "SELECT task_id FROM tasks ORDER BY task_id DESC LIMIT 1"};
    query.exec();

    if (!query.isDone()) {
        return tdl::Err(tdl::DbError::SelectFailure);
    }

    task.set_id(std::stoi(query.getColumn(0)));

    return tdl::Ok();
}

tdl::Result<void, tdl::DbError> TaskDb::update_object(
    const Task& task) const noexcept {
    SQL::Statement query{
        m_db,
        "UPDATE tasks SET topic = ?, content = ?, start_date = ?, deadline "
        "= ?, executor_id = ?, owner_id = ?, chatroom_id = ?, is_done = ? "
        "WHERE task_id = ?"};
    query.bind(1, task.topic());
    query.bind(2, task.content());
    query.bind(3, task.start_date<String>());
    query.bind(4, task.deadline<String>());
    query.bind(5, task.executor_id());
    query.bind(6, task.owner_id());
    query.bind(7, task.chatroom_id());
    query.bind(8, task.is_done());
    query.bind(9, task.id());

    query.exec();
    if (!query.isDone()) {
        return tdl::Err(tdl::DbError::UpdateFailure);
    }

    return tdl::Ok();
}

tdl::Result<void, tdl::DbError> TaskDb::delete_object(
    const Task& task) const noexcept {
    SQL::Statement query{m_db, "DELETE FROM tasks WHERE task_id = ?"};
    query.bind(2, std::to_string(task.id()));

    query.exec();
    if (!query.isDone()) {
        return tdl::Err(tdl::DbError::DeleteFailure);
    }

    return tdl::Ok();
}

MessageDb::MessageDb(StringView db_filepath) {
    m_db = SQL::Database(db_filepath);

    if (!is_table_empty()) {
        SQL::Statement query{
            m_db,
            "CREATE TABLE chatrooms IF NOT EXISTS (chatroom_id INTEGER "
            "AUTOINCREMENT PRIMARY KEY NOT NULL, task_id INTEGER NOT "
            "NULL)"};

        query.exec();
        if (!query.isDone())
            throw std::runtime_error("Failure creating chatrooms table.");
    }

    if (!m_db.tableExists("messages")) {
        SQL::Statement query{
            m_db,
            "CREATE TABLE messages IF NOT EXISTS (message_id INTEGER "
            "AUTOINCREMENT PRIMARY KEY NOT NULL, chatroom_id INTEGER NOT "
            "NULL, sender_name VARCHAR(20) NOT NULL, content TEXT NOT "
            "NULL, timestamp VARCHAR(10) NOT NULL)"};

        query.exec();
        if (!query.isDone())
            throw std::runtime_error("Failure creating messages table.");
    }
}

tdl::Result<Vector<Message>, tdl::DbError> MessageDb::get_all_objects()
    const noexcept {
    SQL::Statement query{m_db, "SELECT * FROM messages"};

    Vector<Message> messages;
    while (query.executeStep()) {
        messages.push_back(Message{query.getColumn(0).getInt(),
                                   query.getColumn(1).getString(),
                                   query.getColumn(2).getString(),
                                   tdl::stotp(query.getColumn(3).getString())});
    }

    if (!query.isDone()) {
        return tdl::Err(tdl::DbError::SelectFailure);
    }

    return tdl::Ok(std::move(messages));
};
bool MessageDb::is_table_empty() const noexcept {
    return m_db.tableExists("chatrooms") && m_db.tableExists("messages");
}

tdl::Result<void, tdl::DbError> MessageDb::add_object(
    Message& message) noexcept {
    SQL::Statement query{m_db,
                         "INSERT INTO messages (chatroom_id, sender_name, "
                         "content, timestamp) VALUES (?, ?, ?, ?)"};
    query.bind(1, message.chatroom_id());
    query.bind(2, String{message.sender_name()});
    query.bind(3, String{message.content()});
    query.bind(4, tdl::tptos(message.timestamp()));

    query.exec();
    if (!query.isDone()) {
        return tdl::Err(tdl::DbError::InsertFailure);
    }

    return tdl::Ok();
};
}  // namespace twodocore