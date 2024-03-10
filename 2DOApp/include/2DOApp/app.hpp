#pragma once

#include <memory>
#include <optional>

#include <fmt/color.h>
#include <fmt/core.h>

#include <2DOCore/task.hpp>
#include <2DOCore/term.hpp>
#include <2DOCore/user.hpp>
#include <Utils/result.hpp>
#include <Utils/type.hpp>
#include <Utils/util.hpp>

namespace tdc = twodocore;
namespace tdu = twodoutils;

#define QUIT_OPTION "0"
#define FIRST_OPTION "1"
#define SECOND_OPTION "2"
#define THIRD_OPTION "3"
#define FOURTH_OPTION "4"
#define FIFTH_OPTION "5"
#define YES "y"
#define NO "n"
#define ENV_FOLDER_NAME "2DO"
#define DB_NAME "2do_db.db3"
#define ERR_LOGS_FILE_NAME "big_error_logs.txt"
#define USER_LOGS_FILE_NAME "user_logs.txt"

namespace twodo {
struct Updated {};
struct Wiped {};

class [[nodiscard]] App {
  public:
    App(App&& other) = default;
    App& operator=(App&& other) = default;
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    App();

    static std::shared_ptr<App> getInstance() {
        if (!instance) {
            instance = std::make_shared<App>();
        }

        return instance;
    }

    std::shared_ptr<App> set_dependencies(
        std::shared_ptr<tdu::IPrinter> iprinter,
        std::shared_ptr<tdu::IUserInputHandler> inputhandler) const {
        instance->m_printer = iprinter;
        instance->m_input_handler = inputhandler;
        return instance;
    }

    void run();

  private:
    inline static std::shared_ptr<App> instance = nullptr;

    std::optional<tdc::User> m_current_user{};
    std::shared_ptr<tdc::UserDb> m_user_db = nullptr;
    std::optional<tdc::TaskDb> m_task_db{};
    std::optional<tdc::MessageDb> m_message_db{};
    std::optional<tdc::AuthenticationManager> m_auth_manager{};

    std::shared_ptr<tdu::IPrinter> m_printer = nullptr;
    std::shared_ptr<tdu::IUserInputHandler> m_input_handler = nullptr;

    enum class UserUpdateEvent {
        UsernameUpdate,
        PasswordUpdate,
        RoleUpdate,
        UserDelete
    };

    enum class TaskUpdateEvent {
        TopicUpdate,
        ContentUpdate,
        DeadlineUpdate,
        ExecutorUpdate,
        TaskDelete
    };

    tdc::Menu load_menu();
    std::shared_ptr<tdc::Page> load_tasks_menu() const;
    std::shared_ptr<tdc::Page> load_create_tasks_menu() const;
    std::shared_ptr<tdc::Page> load_settings_menu();
    std::shared_ptr<tdc::Page> load_user_manager_menu();
    std::shared_ptr<tdc::Page> load_user_update_menu();
    std::shared_ptr<tdc::Page> load_new_user_menu() const;
    std::shared_ptr<tdc::Page> load_advanced_menu() const;

    template <tdc::TaskDb::IdType T>
    void load_update_tasks_menu() const {
        Vector<tdc::Task> tasks;
        if constexpr (T == tdc::TaskDb::IdType::Executor) {
            tasks = m_task_db->get_all_objects<tdc::TaskDb::IdType::Executor>(
                m_current_user->id());
        } else {
            tasks = m_task_db->get_all_objects<tdc::TaskDb::IdType::Owner>(
                m_current_user->id());
        }

        const auto tasks_page = std::make_shared<tdc::Page>("Tasks", [&] {
            unsigned int count = 0;
            for (const auto& task : tasks) {
                m_printer->msg_print(fmt::format(
                    "[{}] {} ({})\n",
                    fmt::format(fg(fmt::color::blue_violet),
                                std::to_string(++count)),
                    fmt::format(fg(fmt::color::blue_violet), task.topic()),
                    (task.is_done()
                         ? fmt::format(fmt::fg(fmt::color::green), "DONE")
                         : fmt::format(fmt::fg(fmt::color::red),
                                       "INCOMPLETE"))));
            }
        });

        unsigned int count = 0;
        for (auto& task : tasks) {
            const auto chosen_task = std::make_shared<tdc::Page>([&] {
                if constexpr (T == tdc::TaskDb::IdType::Executor) {
                    m_printer->msg_print(fmt::format(
                        "Topic: {}\nContent: {}\nDelegated By: {}\nStart "
                        "Date: "
                        "{}\nDeadline: "
                        "{}\nStatus: {}\n\n",
                        task.topic(), task.content(),
                        m_user_db->get_object(task.owner_id()).username(),
                        tdu::to_string(task.start_date<TimePoint>()),
                        tdu::to_string(task.deadline<TimePoint>()),
                        (task.is_done()
                             ? fmt::format(fmt::fg(fmt::color::green), "DONE")
                             : fmt::format(fmt::fg(fmt::color::red),
                                           "INCOMPLETE"))));
                } else {
                    m_printer->msg_print(fmt::format(
                        "Topic: {}\nContent: {}\nDelegated To: {}\nStart "
                        "Date: "
                        "{}\nDeadline: "
                        "{}\nStatus: {}\n\n",
                        task.topic(), task.content(),
                        m_user_db->get_object(task.executor_id()).username(),
                        tdu::to_string(task.start_date<TimePoint>()),
                        tdu::to_string(task.deadline<TimePoint>()),
                        (task.is_done()
                             ? fmt::format(fmt::fg(fmt::color::green), "DONE")
                             : fmt::format(fmt::fg(fmt::color::red),
                                           "INCOMPLETE"))));
                }
            });

            const auto change_status =
                std::make_shared<tdc::Page>("Mark As Complete", false, [&] {
                    if (task_completion_event(task)) {
                        throw Updated{};
                    }
                });

            const auto discussion = std::make_shared<tdc::Page>(
                "Discussion", false, [&] { discussion_event(task); });

            if (!task.is_done() &&
                task.deadline<TimePoint>() > tdu::get_current_timestamp()) {
                chosen_task->attach(FIRST_OPTION, change_status);
                chosen_task->attach(SECOND_OPTION, discussion);
            }

            if constexpr (T == tdc::TaskDb::IdType::Owner) {
                const auto edit_task = std::make_shared<tdc::Page>("Edit Task");

                const auto edit_topic =
                    std::make_shared<tdc::Page>("Edit Topic", false, [&] {
                        if (task_update_event(TaskUpdateEvent::TopicUpdate,
                                              task)) {
                            throw Updated{};
                        }
                    });

                const auto edit_content =
                    std::make_shared<tdc::Page>("Edit Content", false, [&] {
                        if (task_update_event(TaskUpdateEvent::ContentUpdate,
                                              task)) {
                            throw Updated{};
                        }
                    });

                const auto change_deadline =
                    std::make_shared<tdc::Page>("Change Deadline", false, [&] {
                        if (task_update_event(TaskUpdateEvent::DeadlineUpdate,
                                              task)) {
                            throw Updated{};
                        }
                    });

                const auto change_executor =
                    std::make_shared<tdc::Page>("Change Executor", false, [&] {
                        if (task_update_event(TaskUpdateEvent::ExecutorUpdate,
                                              task)) {
                            throw Updated{};
                        }
                    });

                const auto delete_task =
                    std::make_shared<tdc::Page>("Delete Task", false, [&] {
                        if (task_update_event(TaskUpdateEvent::TaskDelete,
                                              task)) {
                            throw Updated{};
                        }
                    });

                if (!task.is_done() &&
                    task.deadline<TimePoint>() > tdu::get_current_timestamp()) {
                    chosen_task->attach(THIRD_OPTION, edit_task);
                }

                edit_task->attach(FIRST_OPTION, edit_topic);
                edit_task->attach(SECOND_OPTION, edit_content);
                edit_task->attach(THIRD_OPTION, change_deadline);
                edit_task->attach(FOURTH_OPTION, change_executor);
                edit_task->attach(FIFTH_OPTION, delete_task);
            }

            tasks_page->attach(std::to_string(++count), chosen_task);
        }

        try {  // ugliness
            tdc::Menu{tasks_page, m_printer, m_input_handler}.run(QUIT_OPTION);
        } catch (const Updated) {
        }
    }

    bool sing_in();
    void sing_up() const;
    bool is_first_user() const;
    bool user_update_event(const UserUpdateEvent kind, tdc::User& user);
    void update_current_user(const tdc::User& user);
    bool task_update_event(const TaskUpdateEvent kind, tdc::Task& task) const;
    bool task_completion_event(tdc::Task& task) const;
    void discussion_event(const tdc::Task& task) const;
    String username_validation_event() const;
    String password_validation_event() const;
    tdc::Role role_choosing_event() const;
    tdc::User executor_choosing_event() const;
    TimePoint datetime_validation_event(StringView msg) const;
    bool privileges_validation_event() const;
    bool privileges_validation_event(const tdc::User& user) const;
    void invalid_option_event() const;
    String string_input(StringView msg) const;
};
}  // namespace twodo