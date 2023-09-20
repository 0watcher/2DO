#include "task.hpp"

namespace twodo
{
TaskDb::TaskDb()
{
    m_db.create_table("tasks", {{"id", "INT PRIMARY KEY AUTOINCREMENT"},
                                {"topic", "TEXT"},
                                {"content", "TEXT"},
                                {"start_date", "TEXT"},
                                {"deadline", "TEXT"},
                                {"eid", "INT"},
                                {"oid", "INT"},
                                {"discussion", "TEXT"},
                                {"done", "BOOL"}});
}

// Result<Task, TaskErr> TaskDb::get_task(const String &topic)
// {
//     auto data = m_db.select_data(
//         "tasks", {"topic", "content", "start_date", "deadline", "eid", "oid", "discussion", "done"},
//         {"topic", topic});
//     if (!data)
//     {
//         return Err<Task, TaskErr>(TaskErr::GetTaskFailure);
//     }
//     return Ok<Task, TaskErr>(Task(data.value()[0], data.value()[1], data.value()[2], data.value()[3], stoi(data.value()[4]), stoi(data.value()[5]), Discussion{}, bool(stoi(data.value()[7])));
// }

// Result<None, TaskErr> TaskDb::add_task(const Task &task)
// {
//     auto result = m_db.insert_data("tasks", {{"topic", task.get_topic()},
//                                              {"content", task.get_content()},
//                                              {"start_date", task.get_start_date()},
//                                              {"deadline", task.get_deadline()},
//                                              {"eid", task.get_executor_id()},
//                                              {"oid", task.get_owner_id()},
//                                              {"discussion", task.get_discussion()},
//                                              {"done", task.get_is_done()}});
//     if (!result)
//     {
//         return Err<None, TaskErr>(TaskErr::AddTaskFailure);
//     }
//     return Ok<None, TaskErr>({});
// }

// Result<None, TaskErr> TaskDb::delete_task(int id)
// {
//     auto result = m_db.delete_data("tasks", {"id", id});
//     if (!result)
//     {
//         return Err<None, TaskErr>(TaskErr::DeleteTaskFailure);
//     }
//     return Ok<None, TaskErr>({});
// }

// Result<None, TaskErr> TaskDb::udpate_data() {}
}  // namespace twodo