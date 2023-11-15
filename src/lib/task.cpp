#include "task.hpp"

#include <string>

#include "result.hpp"

#define TASKS_TABLE "tasks"

namespace twodo
{
TaskDb::TaskDb(const String& path) : m_db {path}
{
    if (!m_db.is_table_empty(TASKS_TABLE))
    {
        auto result = m_db.create_table(TASKS_TABLE, {{"id", "INTEGER PRIMARY KEY AUTOINCREMENT"},
                                                  {"topic", "TEXT"},
                                                  {"content", "TEXT"},
                                                  {"start_date", "TEXT"},
                                                  {"deadline", "TEXT"},
                                                  {"eid", "INTEGER"},
                                                  {"oid", "INTEGER"},
                                                  {"discussion", "TEXT"},
                                                  {"done", "INTEGER"}});
        if (!result)
        {
            throw std::runtime_error("Failed create the table: " + result.err().sql_err());
        }
    }
}

Result<Task, TaskErr> TaskDb::get_task(const String& topic)
{
    auto task = m_db.select_data(
        TASKS_TABLE,
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
        TASKS_TABLE, {"topic", "content", "start_date", "deadline", "eid", "oid", "discussion", "done"},
        {"id", std::to_string(id)});
    if (!task)
    {
        return Err<Task, TaskErr>(TaskErr::GetTaskFailure);
    }
    std::vector<Value> task_data = task.value();

    String topic = task_data[0];
    String content = task_data[1];
    TimePoint start_date = stotp(task_data[2]);
    TimePoint deadline = stotp(task_data[3]);
    int eid = std::stoi(task_data[4]);
    int oid = std::stoi(task_data[5]);
    bool done = (std::stoi(task_data[7]) != 0) ? true : false;

    return Ok<Task, TaskErr>(
        Task {id, topic, content, start_date, deadline, eid, oid, Discussion {}, done});
}

[[nodiscard]] Result<Id, TaskErr> TaskDb::get_task_id(const String& topic)
{
    auto id = m_db.select_data(TASKS_TABLE, {"id"}, {"topic", topic});
    if (!id)
    {
        return Err<int, TaskErr>(TaskErr::GetTaskFailure);
    }
    return Ok<int, TaskErr>(stoi(id.value()[0]));
}

Result<None, TaskErr> TaskDb::add_task(Task& task)
{
    const auto topic = task.get_topic();
    const auto content = task.get_content();
    const auto start_date = tptos(task.get_start_date());
    const auto deadline = tptos(task.get_deadline());
    const auto eid = std::to_string(task.get_executor_id());
    const auto oid = std::to_string(task.get_owner_id());
    const auto done = std::to_string((task.get_is_done())? 1 : 0);

    auto result = m_db.insert_data(TASKS_TABLE, {{"topic", topic},
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

    auto id = get_task_id(task.get_topic());
    if (!id)
    {
        return Err<None, TaskErr>(TaskErr::AddTaskFailure);
    }
    task.set_id(id.value());

    return Ok<None, TaskErr>({});
}

Result<None, TaskErr> TaskDb::delete_task(int id)
{
    auto result = m_db.delete_data(TASKS_TABLE, {"id", std::to_string(id)});
    if (!result)
    {
        return Err<None, TaskErr>(TaskErr::DeleteTaskFailure);
    }
    return Ok<None, TaskErr>({});
}

Result<None, TaskErr> TaskDb::update_data(const Task& task)
{
    auto data = m_db.select_data(
        TASKS_TABLE,
        {"id", "topic", "content", "start_date", "deadline", "eid", "oid", "discussion", "done"},
        {"id", std::to_string(task.get_id())});

    if (!data)
    {
        return Err<None, TaskErr>(TaskErr::GetTaskFailure);
    }

    String db_id = data.value()[0];
    String db_topic = data.value()[1];
    String db_content = data.value()[3];
    String db_start_date = data.value()[4];
    String db_deadline = data.value()[5];
    String db_eid = data.value()[6];
    String db_oid = data.value()[7];
    String db_discussion = data.value()[8];
    String db_done = data.value()[9];

    String id = std::to_string(task.get_id());
    String topic = task.get_topic();
    String content = task.get_content();
    String start_date = tptos(task.get_deadline());
    String deadline = tptos(task.get_deadline());
    String eid = std::to_string(task.get_executor_id());
    String oid = std::to_string(task.get_owner_id());
    String discussion = "";
    String done = std::to_string(task.get_is_done());

    if (id != db_id)
    {
        auto result = m_db.update_data("tasks", {"id", id}, {"id", id});
        if (!result)
        {
            return Err<None, TaskErr>(TaskErr::UpdateTaskFailure);
        }
    }

    if (topic != db_topic)
    {
        auto result = m_db.update_data("tasks", {"topic", topic}, {"id", db_id});
        if (!result)
        {
            return Err<None, TaskErr>(TaskErr::UpdateTaskFailure);
        }
    }

    if (content != db_content)
    {
        auto result = m_db.update_data("tasks", {"content", content}, {"id", db_id});
        if (!result)
        {
            return Err<None, TaskErr>(TaskErr::UpdateTaskFailure);
        }
    }

    if (start_date != db_start_date)
    {
        auto result = m_db.update_data("tasks", {"start_date", start_date}, {"id", db_id});
        if (!result)
        {
            return Err<None, TaskErr>(TaskErr::UpdateTaskFailure);
        }
    }

    if (deadline != db_deadline)
    {
        auto result = m_db.update_data("tasks", {"deadline", deadline}, {"id", db_id});
        if (!result)
        {
            return Err<None, TaskErr>(TaskErr::UpdateTaskFailure);
        }
    }

    if (eid != db_eid)
    {
        auto result = m_db.update_data("tasks", {"eid", eid}, {"id", db_id});
        if (!result)
        {
            return Err<None, TaskErr>(TaskErr::UpdateTaskFailure);
        }
    }

    if (oid != db_oid)
    {
        auto result = m_db.update_data("tasks", {"oid", oid}, {"id", db_id});
        if (!result)
        {
            return Err<None, TaskErr>(TaskErr::UpdateTaskFailure);
        }
    }

    if (discussion != db_discussion)
    {
        auto result = m_db.update_data("tasks", {"discussion", discussion}, {"id", db_id});
        if (!result)
        {
            return Err<None, TaskErr>(TaskErr::UpdateTaskFailure);
        }
    }

    if (done != db_done)
    {
        auto result = m_db.update_data("tasks", {"done", done}, {"id", db_id});
        if (!result)
        {
            return Err<None, TaskErr>(TaskErr::UpdateTaskFailure);
        }
    }

    return Ok<None, TaskErr>({});
}
}  // namespace twodo