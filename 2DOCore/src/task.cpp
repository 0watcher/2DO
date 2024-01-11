#include "2DOCore/task.hpp"

#include <stdexcept>
#include "Utils/type.hpp"

#define TASKS_TABLE "tasks"
#define CHATS_TABLE "chatrooms"
#define MSG_TABLE "messages"

namespace twodocore {
tdl::Result<tdl::None, TaskErr> TaskDb::add_message(Message msg) {
    auto result =
        m_db.insert_data(CHATS_TABLE, {{"sender", msg.sender},
                                       {"content", msg.content},
                                       {"timestamp", tdl::tptos(msg.timestamp)},
                                       {"did", std::to_string(msg.crid)}});
    if (!result) {
        return tdl::Err<tdl::None, TaskErr>(TaskErr::AddTaskFailure);
    }
    return tdl::Ok<tdl::None, TaskErr>({});
}

TaskDb::TaskDb(const String& path) : m_db{path} {
    if (m_db.is_table_empty(TASKS_TABLE)) {
        auto result = m_db.create_table(
            TASKS_TABLE, {{"id", "INTEGER PRIMARY KEY AUTOINCREMENT"},
                          {"topic", "TEXT"},
                          {"content", "TEXT"},
                          {"start_date", "TEXT"},
                          {"deadline", "TEXT"},
                          {"eid", "INTEGER"},
                          {"oid", "INTEGER"},
                          {"did", "INTEGER"},
                          {"done", "INTEGER"}});
        if (!result) {
            throw std::runtime_error("Failed create the table: " +
                                     result.err().sql_err());
        }

        if (m_db.is_table_empty(CHATS_TABLE)) {
            auto msg_result = m_db.create_table(
                MSG_TABLE, {{"id", "INTEGER PRIMARY KEY AUTOINCREMENT"},
                            {"sender", "TEXT"},
                            {"content", "TEXT"},
                            {"timestamp", "TEXT"},
                            {"crid", "INTEGER"}});

            if (!msg_result) {
                throw std::runtime_error("Failed to create the table: " +
                                         msg_result.err().sql_err());
            }

            auto cr_result = m_db.create_table(
                CHATS_TABLE, {{"id", "INTEGER PRIMARY KEY AUTOINCREMENT"},
                              {"tid", "INTEGER"}});

            if (!cr_result) {
                throw std::runtime_error("Failed to create the table: " +
                                         cr_result.err().sql_err());
            }
        }
    }
}

tdl::Result<Task, TaskErr> TaskDb::get_task(const String& topic) {
    auto task = m_db.select_data(TASKS_TABLE,
                                 {"id", "topic", "content", "start_date",
                                  "deadline", "eid", "oid", "did", "done"},
                                 {"topic", topic});
    if (!task) {
        return tdl::Err<Task, TaskErr>(TaskErr::GetTaskFailure);
    }
    Vector<Value> task_data = task.value();
    return tdl::Ok<Task, TaskErr>(
        Task{std::stoi(task_data[0]), task_data[1], task_data[2],
             tdl::stotp(task_data[3]), tdl::stotp(task_data[4]),
             std::stoi(task_data[5]), std::stoi(task_data[6]), 0,
             bool(std::stoi(task_data[7]))});
}

tdl::Result<Task, TaskErr> TaskDb::get_task(int id) {
    auto task = m_db.select_data(TASKS_TABLE,
                                 {"topic", "content", "start_date", "deadline",
                                  "eid", "oid", "did", "done"},
                                 {"id", std::to_string(id)});
    if (!task) {
        return tdl::Err<Task, TaskErr>(TaskErr::GetTaskFailure);
    }
    Vector<Value> task_data = task.value();

    String topic = task_data[0];
    String content = task_data[1];
    TimePoint start_date = tdl::stotp(task_data[2]);
    TimePoint deadline = tdl::stotp(task_data[3]);
    int eid = std::stoi(task_data[4]);
    int oid = std::stoi(task_data[5]);
    bool done = (std::stoi(task_data[7]) != 0) ? true : false;

    return tdl::Ok<Task, TaskErr>(
        Task{id, topic, content, start_date, deadline, eid, oid, 0, done});
}

[[nodiscard]] tdl::Result<Id, TaskErr> TaskDb::get_task_id(
    const String& topic) {
    auto id = m_db.select_data(TASKS_TABLE, {"id"}, {"topic", topic});
    if (!id) {
        return tdl::Err<int, TaskErr>(TaskErr::GetTaskFailure);
    }
    return tdl::Ok<int, TaskErr>(stoi(id.value()[0]));
}

tdl::Result<tdl::None, TaskErr> TaskDb::add_task(Task& task) {
    const auto topic = task.topic();
    const auto content = task.content();
    const auto start_date = tdl::tptos(task.start_date());
    const auto deadline = tdl::tptos(task.deadline());
    const auto eid = std::to_string(task.executor_id());
    const auto oid = std::to_string(task.owner_id());
    const auto done = std::to_string((task.is_done()) ? 1 : 0);

    auto result = m_db.insert_data(TASKS_TABLE, {{"topic", topic},
                                                 {"content", content},
                                                 {"start_date", start_date},
                                                 {"deadline", deadline},
                                                 {"eid", eid},
                                                 {"oid", oid},
                                                 {"did", std::to_string(0)},
                                                 {"done", done}});
    if (!result) {
        return tdl::Err<tdl::None, TaskErr>(TaskErr::AddTaskFailure);
    }
    auto id = get_task_id(task.topic());
    if (!id) {
        return tdl::Err<tdl::None, TaskErr>(TaskErr::AddTaskFailure);
    }
    task.set_id(id.value());

    return tdl::Ok<tdl::None, TaskErr>({});
}

tdl::Result<tdl::None, TaskErr> TaskDb::delete_task(int id) {
    auto result = m_db.delete_data(TASKS_TABLE, {"id", std::to_string(id)});
    if (!result) {
        return tdl::Err<tdl::None, TaskErr>(TaskErr::DeleteTaskFailure);
    }
    return tdl::Ok<tdl::None, TaskErr>({});
}

tdl::Result<tdl::None, TaskErr> TaskDb::update_data(const Task& task) {
    auto data = m_db.select_data(TASKS_TABLE,
                                 {"id", "topic", "content", "start_date",
                                  "deadline", "eid", "oid", "did", "done"},
                                 {"id", std::to_string(task.id())});

    if (!data) {
        return tdl::Err<tdl::None, TaskErr>(TaskErr::GetTaskFailure);
    }

    String db_id = data.value()[0];
    String db_topic = data.value()[1];
    String db_content = data.value()[3];
    String db_start_date = data.value()[4];
    String db_deadline = data.value()[5];
    String db_eid = data.value()[6];
    String db_oid = data.value()[7];
    String db_did = data.value()[8];
    String db_done = data.value()[9];

    String id = std::to_string(task.id());
    String topic = task.topic();
    String content = task.content();
    String start_date = tdl::tptos(task.deadline());
    String deadline = tdl::tptos(task.deadline());
    String eid = std::to_string(task.executor_id());
    String oid = std::to_string(task.owner_id());
    String did = 0;
    String done = std::to_string(task.is_done());

    if (id != db_id) {
        auto result = m_db.update_data("tasks", {"id", id}, {"id", id});
        if (!result) {
            return tdl::Err<tdl::None, TaskErr>(TaskErr::UpdateTaskFailure);
        }
    }

    if (topic != db_topic) {
        auto result =
            m_db.update_data("tasks", {"topic", topic}, {"id", db_id});
        if (!result) {
            return tdl::Err<tdl::None, TaskErr>(TaskErr::UpdateTaskFailure);
        }
    }

    if (content != db_content) {
        auto result =
            m_db.update_data("tasks", {"content", content}, {"id", db_id});
        if (!result) {
            return tdl::Err<tdl::None, TaskErr>(TaskErr::UpdateTaskFailure);
        }
    }

    if (start_date != db_start_date) {
        auto result = m_db.update_data("tasks", {"start_date", start_date},
                                       {"id", db_id});
        if (!result) {
            return tdl::Err<tdl::None, TaskErr>(TaskErr::UpdateTaskFailure);
        }
    }

    if (deadline != db_deadline) {
        auto result =
            m_db.update_data("tasks", {"deadline", deadline}, {"id", db_id});
        if (!result) {
            return tdl::Err<tdl::None, TaskErr>(TaskErr::UpdateTaskFailure);
        }
    }

    if (eid != db_eid) {
        auto result = m_db.update_data("tasks", {"eid", eid}, {"id", db_id});
        if (!result) {
            return tdl::Err<tdl::None, TaskErr>(TaskErr::UpdateTaskFailure);
        }
    }

    if (oid != db_oid) {
        auto result = m_db.update_data("tasks", {"oid", oid}, {"id", db_id});
        if (!result) {
            return tdl::Err<tdl::None, TaskErr>(TaskErr::UpdateTaskFailure);
        }
    }

    if (did != db_did) {
        auto result = m_db.update_data("tasks", {"did", did}, {"id", db_id});
        if (!result) {
            return tdl::Err<tdl::None, TaskErr>(TaskErr::UpdateTaskFailure);
        }
    }

    if (done != db_done) {
        auto result = m_db.update_data("tasks", {"done", done}, {"id", db_id});
        if (!result) {
            return tdl::Err<tdl::None, TaskErr>(TaskErr::UpdateTaskFailure);
        }
    }

    return tdl::Ok<tdl::None, TaskErr>({});
}
}  // namespace twodocore