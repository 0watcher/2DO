#pragma once

#include <cstddef>
#include <memory>

#include <2DOApp/term.hpp>
#include <2DOCore/task.hpp>
#include <2DOCore/user.hpp>
#include <Utils/result.hpp>
#include <Utils/type.hpp>
#include "Utils/util.hpp"

namespace tdc = twodocore;
namespace tdu = twodoutils;

#define QUIT_OPTION "0"
#define FIRST_OPTION "1"
#define SECOND_OPTION "2"
#define THIRD_OPTION "3"
#define FOURTH_OPTION "4"
#define YES "y"
#define NO "n"
#define DB_NAME ":memory:"
#define ERR_LOGS_FILE_NAME "big-error-logs.txt"
#define USER_LOGS_FILE_NAME "user-logs.txt"

namespace twodo {
class [[nodiscard]] App {
  public:
    App(App&& other) = default;
    App& operator=(App&& other) = default;
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    App() = default;

    static std::shared_ptr<App> getInstance() {
        if (!instance) {
            instance = std::make_shared<App>();
        }
        return instance;
    }

    std::shared_ptr<App> set_dependencies(
        std::shared_ptr<tdu::IPrinter> iprinter,
        std::shared_ptr<tdu::IUserInputHandler> inputhandler) {
        instance->m_printer = iprinter;
        instance->m_input_handler = inputhandler;
        return instance;
    }

    void run();

  private:
    inline static std::shared_ptr<App> instance = nullptr;
    std::shared_ptr<tdc::User> m_current_user = nullptr;
    std::shared_ptr<tdc::UserDb> m_user_db =
        std::make_shared<tdc::UserDb>(DB_NAME);
    std::shared_ptr<tdc::TaskDb> m_task_db =
        std::make_shared<tdc::TaskDb>(DB_NAME);
    std::shared_ptr<tdc::MessageDb> m_message_db =
        std::make_shared<tdc::MessageDb>(DB_NAME);
    tdc::AuthenticationManager m_amanager{m_user_db};
    std::shared_ptr<tdu::IPrinter> m_printer = nullptr;
    std::shared_ptr<tdu::IUserInputHandler> m_input_handler = nullptr;
    tdu::Resource m_resource{};

    enum class UpdateEvent {
        UsernameUpdate,
        PasswordUpdate,
        RoleUpdate,
        UserDelete
    };

    Menu load_menu();
    std::shared_ptr<Page> load_main_menu() const;

    std::shared_ptr<Page> load_tasks_menu() const;
    std::shared_ptr<Page> load_your_tasks_menu() const;
    std::shared_ptr<Page> load_delegated_tasks_menu() const;
    std::shared_ptr<Page> load_create_tasks_menu() const;

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

        const auto tasks_page = std::make_shared<Page>([this, &tasks] {
            if constexpr (T == tdc::TaskDb::IdType::Executor) {
                m_printer->msg_print("Your tasks:\n");
            } else {
                m_printer->msg_print("Delegated tasks:\n");
            }

            unsigned int count = 0;
            for (const auto& task : tasks) {
                const auto owner = m_user_db->get_object(task.owner_id());

                if constexpr (T == tdc::TaskDb::IdType::Executor) {
                    m_printer->msg_print(fmt::format(
                        "[{}] {} from {} ({})\n", ++count, task.topic(),
                        owner.username(),
                        (task.is_done()
                             ? fmt::format(fmt::fg(fmt::color::green), "DONE")
                             : fmt::format(fmt::fg(fmt::color::red),
                                           "INCOMPLETE"))));
                } else {
                    m_printer->msg_print(fmt::format(
                        "[{}] {} to {} ({})\n", ++count, task.topic(),
                        m_user_db->get_object(task.executor_id()).username(),
                        (task.is_done()
                             ? fmt::format(fmt::fg(fmt::color::green), "DONE")
                             : fmt::format(fmt::fg(fmt::color::red),
                                           "INCOMPLETE"))));
                }
            }
            m_printer->msg_print("\n-> ");
        });

        unsigned int count = 0;
        for (auto& task : tasks) {
            const auto chosen_task = std::make_shared<Page>([this, &task] {
                if constexpr (T == tdc::TaskDb::IdType::Executor) {
                    m_printer->msg_print(fmt::format(
                        "Topic: {}\nContent: {}\nDelegated by: {}\nStart Date: "
                        "{}\nDeadline: "
                        "{}\nStatus: {}\n",
                        task.topic(), task.content(),
                        m_user_db->get_object(task.owner_id()).username(),
                        task.start_date<String>(), task.deadline<String>(),
                        (task.is_done()
                             ? fmt::format(fmt::fg(fmt::color::green), "DONE")
                             : fmt::format(fmt::fg(fmt::color::red),
                                           "INCOMPLETE"))));

                    m_printer->msg_print(
                        "[1] Mark as complete\n"
                        "[2] Discussion\n"
                        "[0] Back\n"
                        "-> ");
                } else {
                    m_printer->msg_print(fmt::format(
                        "Topic: {}\nContent: {}\nDelegated to: {}\nStart Date: "
                        "{}\nDeadline: "
                        "{}\nStatus: {}\n",
                        task.topic(), task.content(),
                        m_user_db->get_object(task.executor_id()).username(),
                        task.start_date<String>(), task.deadline<String>(),
                        (task.is_done()
                             ? fmt::format(fmt::fg(fmt::color::green), "DONE")
                             : fmt::format(fmt::fg(fmt::color::red),
                                           "INCOMPLETE"))));

                    m_printer->msg_print(
                        "[1] Mark as complete\n"
                        "[2] Discussion\n"
                        "[3] Edit\n"
                        "[0] Back\n"
                        "-> ");
                }
            });

            const auto change_status = std::make_shared<Page>(false, [this,
                                                                      &task] {
                tdu::clear_term();
                m_printer->msg_print("Are you 100% sure ? [y/n]\n");
                const auto choice = m_input_handler->get_input();
                if (choice == "y") {
                    task.set_is_done(true);
                    m_task_db->update_object(task);
                    m_printer->msg_print("Congrats you've completed the task!");
                } else if (choice == "n") {
                    return;
                } else {
                    invalid_option_event();
                    return;
                }
            });

            const auto discussion = std::make_shared<Page>(false, [this,
                                                                   &task] {
                while (true) {
                    tdu::clear_term();

                    const auto messages =
                        m_message_db->get_all_objects(task.id());

                    for (const auto& msg : messages) {
                        m_printer->msg_print(fmt::format(
                            "[{}] <{}>: {}\n", msg.timestamp<String>(),
                            msg.sender_name(), msg.content()));
                    }

                    m_printer->msg_print(
                        fmt::format("<{}>: ", m_current_user->username()));
                    String sended_message = m_input_handler->get_input();
                    if (sended_message == "0") {
                        break;
                    }

                    const auto prepared_msg = tdc::Message{
                        task.id(), m_current_user->username(), sended_message,
                        tdu::get_current_timestamp<TimePoint>()};
                    m_message_db->add_object(prepared_msg);

                    m_printer->msg_print(fmt::format(
                        "[{}] <{}>: {}\n", prepared_msg.timestamp<String>(),
                        prepared_msg.sender_name(), prepared_msg.content()));
                }
            });

            if constexpr (T == tdc::TaskDb::IdType::Owner) {
                const auto edit_task = std::make_shared<Page>([this, &task] {
                    m_printer->msg_print(
                        "Edit task:\n"
                        "[1] Edit topic\n"
                        "[2] Edit content\n"
                        "[3] Change deadline\n"
                        "[4] Change executor\n"
                        "[0] Back\n"
                        "-> ");
                });

                const auto edit_topic =
                    std::make_shared<Page>(false, [this, &task] {
                        tdu::clear_term();
                        m_printer->msg_print("topic: ");
                        task.set_topic(m_input_handler->get_input());
                        m_task_db->update_object(task);
                    });

                const auto edit_content =
                    std::make_shared<Page>(false, [this, &task] {
                        tdu::clear_term();
                        m_printer->msg_print("content: ");
                        task.set_content(m_input_handler->get_input());
                        m_task_db->update_object(task);
                    });

                const auto change_deadline =
                    std::make_shared<Page>(false, [this, &task] {
                        TimePoint start_date;

                        while (true) {
                            tdu::clear_term();

                            m_printer->msg_print("deadline: ");
                            try {
                                start_date =
                                    tdu::stotp(m_input_handler->get_input());
                                break;
                            } catch (...) {
                                invalid_option_event();
                            }
                        }

                        task.set_deadline(start_date);
                        m_task_db->update_object(task);
                    });

                const auto change_executor =
                    std::make_shared<Page>(false, [this, &task] {
                        const auto users = m_user_db->get_all_objects();

                        while (true) {
                            tdu::clear_term();

                            m_printer->msg_print("assign executor:\n");

                            for (size_t count = 1; count < users.size();
                                 ++count) {
                                m_printer->msg_print(
                                    fmt::format("[{}] {} <{}>\n", count,
                                                users[count].username(),
                                                users[count].role<String>()));
                            }
                            m_printer->msg_print("-> ");

                            if (unsigned const int executor_number =
                                    std::stoi(m_input_handler->get_input());
                                executor_number > 0 &&
                                executor_number <= users.size()) {
                                task.set_executor(
                                    users[executor_number - 1].id());
                                m_task_db->update_object(task);
                                break;
                            }

                            invalid_option_event();
                        }
                    });

                chosen_task->attach(THIRD_OPTION, edit_task);

                edit_task->attach(FIRST_OPTION, edit_topic);
                edit_task->attach(SECOND_OPTION, edit_content);
                edit_task->attach(THIRD_OPTION, change_deadline);
                edit_task->attach(FOURTH_OPTION, change_executor);
            }

            chosen_task->attach(FIRST_OPTION, change_status);
            chosen_task->attach(SECOND_OPTION, discussion);

            tasks_page->attach(std::to_string(++count), chosen_task);
        }

        auto your_tasks_menu = Menu{tasks_page, m_printer, m_input_handler};

        your_tasks_menu.run(QUIT_OPTION);
    }

    std::shared_ptr<Page> load_settings_menu();
    std::shared_ptr<Page> load_user_manager_menu();
    std::shared_ptr<Page> load_user_update_menu();
    std::shared_ptr<Page> load_new_user_menu();
    std::shared_ptr<Page> load_advanced_menu();

    bool user_update_event(UpdateEvent kind, tdc::User& user);
    String username_validation_event();
    String password_validation_event();
    void invalid_option_event() const;
    bool privileges_validation_event() const;
    bool privileges_validation_event(const tdc::User& user) const;

    std::shared_ptr<tdc::User> get_current_user() { return m_current_user; }
};
}  // namespace twodo