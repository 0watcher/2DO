#include "task.hpp"

#include "result.hpp"

namespace twodo
{
TaskDb::TaskDb(const String& path) : m_db {path}
{
    if (!m_db.is_table_empty("tasks"))
    {
        auto result = m_db.create_table("tasks", {{"id", "INT PRIMARY KEY AUTOINCREMENT"},
                                                  {"topic", "TEXT"},
                                                  {"content", "TEXT"},
                                                  {"start_date", "TEXT"},
                                                  {"deadline", "TEXT"},
                                                  {"eid", "INT"},
                                                  {"oid", "INT"},
                                                  {"discussion", "TEXT"},
                                                  {"done", "INT"}});
        if (!result)
        {
            throw std::runtime_error("Failed create the table: " + result.err().sql_err());
        }
    }
}

Result<Task, TaskErr> TaskDb::get_task(const String& topic)
{
    auto task = m_db.select_data(
        "tasks",
        {"id", "topic", "content", "start_date", "deadline", "eid", "oid", "discussion", "done"},
        {"topic", topic});
    if (!task)
    {
        return Err<Task, TaskErr>(TaskErr::GetTaskFailure);
    }
    std::vector<Value> task_data = task.value();
    return Ok<Task, TaskErr>(Task {std::stoi(task_data[0]), task_data[1], task_data[2],
                                   stotp(task_data[3]), stotp(task_data[4]),
                                   std::stoi(task_data[5]), std::stoi(task_data[6]), Discussion {},
                                   bool(std::stoi(task_data[7]))});
}

Result<Task, TaskErr> TaskDb::get_task(int id)
{
    auto task = m_db.select_data(
        "tasks", {"topic", "content", "start_date", "deadline", "eid", "oid", "discussion", "done"},
        {"id", std::to_string(id)});
    if (!task)
    {
        return Err<Task, TaskErr>(TaskErr::GetTaskFailure);
    }
    std::vector<Value> task_data = task.value();
    return Ok<Task, TaskErr>(Task {id, task_data[1], task_data[2], stotp(task_data[3]),
                                   stotp(task_data[4]), std::stoi(task_data[5]),
                                   std::stoi(task_data[6]), Discussion {},
                                   bool(std::stoi(task_data[7]))});
}

[[nodiscard]] Result<Id, TaskErr> TaskDb::get_task_id(const String& topic)
{
    auto id = m_db.select_data("users", {"id"}, {"topic", topic});
    if (!id)
    {
        return Err<int, TaskErr>(TaskErr::GetTaskFailure);
    }
    return Ok<int, TaskErr>(stoi(id.value()[0]));
}

Result<None, TaskErr> TaskDb::add_task(const Task& task)
{
    const auto topic = task.get_topic();
    const auto content = task.get_content();
    const auto start_date = tptos(task.get_start_date());
    const auto deadline = tptos(task.get_deadline());
    const auto eid = std::to_string(task.get_executor_id());
    const auto oid = std::to_string(task.get_owner_id());
    const auto done = std::to_string(int(task.get_is_done()));

    auto result = m_db.insert_data("tasks", {{"topic", topic},
                                             {"content", content},
                                             {"start_date", start_date},
                                             {"deadline", deadline},
                                             {"eid", eid},
                                             {"oid", oid},
                                             {"discussion", ""},
                                             {"done", done}});
    if (!result)
    {
        return Err<None, TaskErr>(TaskErr::AddTaskFailure);
    }

    return Ok<None, TaskErr>({});
}

Result<None, TaskErr> TaskDb::delete_task(int id)
{
    auto result = m_db.delete_data("tasks", {"id", std::to_string(id)});
    if(!result)
    {
        return Err<None, TaskErr>(TaskErr::DeleteTaskFailure);
    }
    return Ok<None, TaskErr>({});
}

Result<None, TaskErr> TaskDb::update_data(const Task& task)
{

}

String tptos(const TimePoint& tp)
{
    std::time_t time = std::chrono::system_clock::to_time_t(tp);

    std::string timeString = std::to_string(time);

    return timeString;
}

TimePoint stotp(const String& stringified_tp)
{
    std::time_t time = std::stoll(stringified_tp);

    TimePoint tp = std::chrono::system_clock::from_time_t(time);

    return tp;
}
}  // namespace twodo