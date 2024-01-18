#include "2DOCore/task.hpp"

#include <stdexcept>
#include "Utils/type.hpp"

#define TASKS_TABLE "tasks"
#define CHATS_TABLE "chatrooms"
#define MSG_TABLE "messages"

namespace twodocore {
// TaskDb::TaskDb(const String& path) : m_db{path} {
//     if (m_db.is_table_empty(TASKS_TABLE)) {
//         const auto result = m_db.create_table(
//             TASKS_TABLE, {{"id", "INTEGER PRIMARY KEY AUTOINCREMENT"},
//                           {"topic", "TEXT"},
//                           {"content", "TEXT"},
//                           {"start_date", "TEXT"},
//                           {"deadline", "TEXT"},
//                           {"eid", "INTEGER"},
//                           {"oid", "INTEGER"},
//                           {"did", "INTEGER"},
//                           {"done", "INTEGER"}});
//         if (!result) {
//             throw std::runtime_error("Failed create the table: " +
//                                      result.err().sql_err());
//         }

//         if (m_db.is_table_empty(CHATS_TABLE)) {
//             const auto msg_result = m_db.create_table(
//                 MSG_TABLE, {{"id", "INTEGER PRIMARY KEY AUTOINCREMENT"},
//                             {"sender", "TEXT"},
//                             {"content", "TEXT"},
//                             {"timestamp", "TEXT"},
//                             {"crid", "INTEGER"}});

//             if (!msg_result) {
//                 throw std::runtime_error("Failed to create the table: " +
//                                          msg_result.err().sql_err());
//             }

//             const auto cr_result = m_db.create_table(
//                 CHATS_TABLE, {{"id", "INTEGER PRIMARY KEY AUTOINCREMENT"},
//                               {"tid", "INTEGER"}});

//             if (!cr_result) {
//                 throw std::runtime_error("Failed to create the table: " +
//                                          cr_result.err().sql_err());
//             }
//         }
//     }
// }

// tdl::Result<Task, TaskErr> TaskDb::get_task(const String& topic) const {
//     const auto task =
//         m_db.select_data(TASKS_TABLE,
//                          {"id", "topic", "content", "start_date", "deadline",
//                           "eid", "oid", "did", "done"},
//                          {"topic", topic});
//     if (!task) {
//         return tdl::Err(TaskErr::GetTaskFailure);
//     }
//     const Vector<tdl::Value> task_data = task.unwrap();
//     return tdl::Ok(Task{std::stoi(task_data[0]), task_data[1], task_data[2],
//                         tdl::stotp(task_data[3]), tdl::stotp(task_data[4]),
//                         std::stoi(task_data[5]), std::stoi(task_data[6]), 0,
//                         bool(std::stoi(task_data[7]))});
// }

// tdl::Result<Task, TaskErr> TaskDb::get_task(int id) const {
//     const auto task =
//         m_db.select_data(TASKS_TABLE,
//                          {"topic", "content", "start_date", "deadline", "eid",
//                           "oid", "did", "done"},
//                          {"id", std::to_string(id)});
//     if (!task) {
//         return tdl::Err(TaskErr::GetTaskFailure);
//     }
//     const Vector<tdl::Value> task_data = task.unwrap();

//     const String topic = task_data[0];
//     const String content = task_data[1];
//     const TimePoint start_date = tdl::stotp(task_data[2]);
//     const TimePoint deadline = tdl::stotp(task_data[3]);
//     const int eid = std::stoi(task_data[4]);
//     const int oid = std::stoi(task_data[5]);
//     const bool done = (std::stoi(task_data[7]) != 0) ? true : false;

//     return tdl::Ok(
//         Task{id, topic, content, start_date, deadline, eid, oid, 0, done});
// }

// [[nodiscard]] tdl::Result<tdl::Id, TaskErr> TaskDb::get_task_id(
//     const String& topic) const {
//     const auto id = m_db.select_data(TASKS_TABLE, {"id"}, {"topic", topic});
//     if (!id) {
//         return tdl::Err(TaskErr::GetTaskFailure);
//     }
//     return tdl::Ok(stoi(id.unwrap()[0]));
// }

// tdl::Result<void, TaskErr> TaskDb::add_task(Task& task) {
//     const auto topic = task.topic();
//     const auto content = task.content();
//     const auto start_date = tdl::tptos(task.start_date());
//     const auto deadline = tdl::tptos(task.deadline());
//     const auto eid = std::to_string(task.executor_id());
//     const auto oid = std::to_string(task.owner_id());
//     const auto done = std::to_string((task.is_done()) ? 1 : 0);

//     const auto result =
//         m_db.insert_data(TASKS_TABLE, {{"topic", topic},
//                                        {"content", content},
//                                        {"start_date", start_date},
//                                        {"deadline", deadline},
//                                        {"eid", eid},
//                                        {"oid", oid},
//                                        {"did", std::to_string(0)},
//                                        {"done", done}});
//     if (!result) {
//         return tdl::Err(TaskErr::AddTaskFailure);
//     }
//     const auto id = get_task_id(task.topic());
//     if (!id) {
//         return tdl::Err(TaskErr::AddTaskFailure);
//     }
//     task.set_id(id.unwrap());

//     return tdl::Ok();
// }

// tdl::Result<void, TaskErr> TaskDb::delete_task(int id) {
//     const auto result =
//         m_db.delete_data(TASKS_TABLE, {"id", std::to_string(id)});
//     if (!result) {
//         return tdl::Err(TaskErr::DeleteTaskFailure);
//     }
//     return tdl::Ok();
// }

// tdl::Result<void, TaskErr> TaskDb::update_data(const Task& task) {
//     const auto data =
//         m_db.select_data(TASKS_TABLE,
//                          {"id", "topic", "content", "start_date", "deadline",
//                           "eid", "oid", "did", "done"},
//                          {"id", std::to_string(task.id())});

//     if (!data) {
//         return tdl::Err(TaskErr::GetTaskFailure);
//     }

//     const String db_id = data.unwrap()[0];
//     const String db_topic = data.unwrap()[1];
//     const String db_content = data.unwrap()[3];
//     const String db_start_date = data.unwrap()[4];
//     const String db_deadline = data.unwrap()[5];
//     const String db_eid = data.unwrap()[6];
//     const String db_oid = data.unwrap()[7];
//     const String db_did = data.unwrap()[8];
//     const String db_done = data.unwrap()[9];

//     const String id = std::to_string(task.id());
//     const String topic = task.topic();
//     const String content = task.content();
//     const String start_date = tdl::tptos(task.deadline());
//     const String deadline = tdl::tptos(task.deadline());
//     const String eid = std::to_string(task.executor_id());
//     const String oid = std::to_string(task.owner_id());
//     const String did = 0;
//     const String done = std::to_string(task.is_done());

//     if (id != db_id) {
//         const auto result = m_db.update_data("tasks", {"id", id}, {"id", id});
//         if (!result) {
//             return tdl::Err(TaskErr::UpdateTaskFailure);
//         }
//     }

//     if (topic != db_topic) {
//         const auto result =
//             m_db.update_data("tasks", {"topic", topic}, {"id", db_id});
//         if (!result) {
//             return tdl::Err(TaskErr::UpdateTaskFailure);
//         }
//     }

//     if (content != db_content) {
//         const auto result =
//             m_db.update_data("tasks", {"content", content}, {"id", db_id});
//         if (!result) {
//             return tdl::Err(TaskErr::UpdateTaskFailure);
//         }
//     }

//     if (start_date != db_start_date) {
//         const auto result = m_db.update_data(
//             "tasks", {"start_date", start_date}, {"id", db_id});
//         if (!result) {
//             return tdl::Err(TaskErr::UpdateTaskFailure);
//         }
//     }

//     if (deadline != db_deadline) {
//         const auto result =
//             m_db.update_data("tasks", {"deadline", deadline}, {"id", db_id});
//         if (!result) {
//             return tdl::Err(TaskErr::UpdateTaskFailure);
//         }
//     }

//     if (eid != db_eid) {
//         const auto result =
//             m_db.update_data("tasks", {"eid", eid}, {"id", db_id});
//         if (!result) {
//             return tdl::Err(TaskErr::UpdateTaskFailure);
//         }
//     }

//     if (oid != db_oid) {
//         const auto result =
//             m_db.update_data("tasks", {"oid", oid}, {"id", db_id});
//         if (!result) {
//             return tdl::Err(TaskErr::UpdateTaskFailure);
//         }
//     }

//     if (did != db_did) {
//         const auto result =
//             m_db.update_data("tasks", {"did", did}, {"id", db_id});
//         if (!result) {
//             return tdl::Err(TaskErr::UpdateTaskFailure);
//         }
//     }

//     if (done != db_done) {
//         const auto result =
//             m_db.update_data("tasks", {"done", done}, {"id", db_id});
//         if (!result) {
//             return tdl::Err(TaskErr::UpdateTaskFailure);
//         }
//     }

//     return tdl::Ok();
// }

// tdl::Result<void, TaskErr> TaskDb::add_message(Message msg) {
//     const auto result =
//         m_db.insert_data(CHATS_TABLE, {{"sender", msg.sender},
//                                        {"content", msg.content},
//                                        {"timestamp", tdl::tptos(msg.timestamp)},
//                                        {"did", std::to_string(msg.crid)}});
//     if (!result) {
//         return tdl::Err(TaskErr::AddTaskFailure);
//     }
//     return tdl::Ok();
// }
}  // namespace twodocore