#pragma once

#include <memory>

#include <fmt/color.h>
#include <fmt/core.h>

#include <2DOApp/term.hpp>
#include <2DOCore/task.hpp>
#include <2DOCore/user.hpp>
#include <Utils/result.hpp>
#include <Utils/type.hpp>
#include <Utils/util.hpp>
#include <stdexcept>

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

    explicit App() = default;

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

    std::shared_ptr<tdc::User> m_current_user = nullptr;

    std::shared_ptr<tdc::UserDb> m_user_db =
        std::make_shared<tdc::UserDb>(DB_NAME);

    std::shared_ptr<tdc::TaskDb> m_task_db =
        std::make_shared<tdc::TaskDb>(DB_NAME);

    std::shared_ptr<tdc::MessageDb> m_message_db =
        std::make_shared<tdc::MessageDb>(DB_NAME);

    tdc::AuthenticationManager m_auth_manager{m_user_db};

    std::shared_ptr<tdu::IPrinter> m_printer = nullptr;

    std::shared_ptr<tdu::IUserInputHandler> m_input_handler = nullptr;

    enum class UpdateEvent {
        UsernameUpdate,
        PasswordUpdate,
        RoleUpdate,
        UserDelete
    };

    Menu load_menu();
    std::shared_ptr<Page> load_tasks_menu() const;
    std::shared_ptr<Page> load_create_tasks_menu() const;
    std::shared_ptr<Page> load_settings_menu() const;
    std::shared_ptr<Page> load_user_manager_menu() const;
    std::shared_ptr<Page> load_user_update_menu() const;
    std::shared_ptr<Page> load_new_user_menu() const;
    std::shared_ptr<Page> load_advanced_menu() const;

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

        const auto tasks_page = std::make_shared<Page>("Tasks", [this, &tasks] {
            unsigned int count = 0;
            for (const auto& task : tasks) {
                m_printer->msg_print(fmt::format(
                    "[{}] {} ({})",
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
            const auto chosen_task = std::make_shared<Page>([this, &task] {
                if constexpr (T == tdc::TaskDb::IdType::Executor) {
                    m_printer->msg_print(fmt::format(
                        "Topic: {}\nContent: {}\nDelegated by: {}\nStart "
                        "Date: "
                        "{}\nDeadline: "
                        "{}\nStatus: {}\n",
                        task.topic(), task.content(),
                        m_user_db->get_object(task.owner_id()).username(),
                        tdu::format_datetime(task.start_date<TimePoint>()),
                        tdu::format_datetime(task.deadline<TimePoint>()),
                        (task.is_done()
                             ? fmt::format(fmt::fg(fmt::color::green), "DONE")
                             : fmt::format(fmt::fg(fmt::color::red),
                                           "INCOMPLETE"))));
                } else {
                    m_printer->msg_print(fmt::format(
                        "Topic: {}\nContent: {}\nDelegated to: {}\nStart "
                        "Date: "
                        "{}\nDeadline: "
                        "{}\nStatus: {}\n",
                        task.topic(), task.content(),
                        m_user_db->get_object(task.executor_id()).username(),
                        tdu::format_datetime(task.start_date<TimePoint>()),
                        tdu::format_datetime(task.deadline<TimePoint>()),
                        (task.is_done()
                             ? fmt::format(fmt::fg(fmt::color::green), "DONE")
                             : fmt::format(fmt::fg(fmt::color::red),
                                           "INCOMPLETE"))));
                }
            });

            const auto change_status = std::make_shared<Page>(
                "Mark as complete", false, [this, &task] {
                    tdu::clear_term();

                    m_printer->msg_print("Are you 100% sure ? [y/n]\n");
                    const auto choice = m_input_handler->get_input();

                    if (choice == "y") {
                        task.set_is_done(true);
                        m_task_db->update_object(task);

                        m_printer->msg_print(
                            "Congrats you've completed the task!");
                    } else if (choice == "n") {
                        return;
                    } else {
                        invalid_option_event();
                        return;
                    }
                });

            const auto discussion =
                std::make_shared<Page>("Discussion", false, [this, &task] {
                    Vector<tdc::Message> messages{};
                    bool should_close = false;

                    const auto main_chat_loop = [this, &task, &messages,
                                                 &should_close] {
                        tdu::clear_term();

                        while (true) {
                            m_printer->msg_print(fmt::format(
                                "<{}>: ", m_current_user->username()));
                            String sended_message =
                                m_input_handler->get_input();
                            if (sended_message == "0") {
                                should_close = true;
                                break;
                            }

                            const auto prepared_msg = tdc::Message{
                                task.id(), m_current_user->username(),
                                sended_message, tdu::get_current_timestamp()};

                            m_message_db->add_object(prepared_msg);
                            // m_printer->msg_print(fmt::format(
                            //     "[{}] <{}>: {}\n",
                            //     tdu::format_datetime(
                            //         prepared_msg.timestamp<TimePoint>()),
                            //     prepared_msg.sender_name(),
                            //     prepared_msg.content()));
                        }
                    };

                    const auto check_for_msg = [this, &messages, &task,
                                                &should_close] {
                        while (!should_close) {
                            const auto new_msg =
                                m_message_db->get_newest_object();

                            if (messages.empty() ||
                                (new_msg.message_id() >
                                 messages.back().message_id())) {
                                tdu::clear_term();

                                messages =
                                    m_message_db->get_all_objects(task.id());
                                for (const auto& msg : messages) {
                                    m_printer->msg_print(fmt::format(
                                        "[{}] <{}>: {}\n",
                                        tdu::format_datetime(
                                            msg.timestamp<TimePoint>()),
                                        msg.sender_name(), msg.content()));
                                }
                            }

                            tdu::sleep(100);
                        }
                    };

                    tdu::fork_tasks(main_chat_loop, check_for_msg);
                });

            if (!task.is_done() &&
                task.deadline<TimePoint>() > tdu::get_current_timestamp()) {
                chosen_task->attach(FIRST_OPTION, change_status);
                chosen_task->attach(SECOND_OPTION, discussion);
            }

            if constexpr (T == tdc::TaskDb::IdType::Owner) {
                const auto edit_task = std::make_shared<Page>("Edit Task");

                const auto edit_topic =
                    std::make_shared<Page>("Edit topic", false, [this, &task] {
                        tdu::clear_term();

                        m_printer->msg_print("topic: ");
                        task.set_topic(m_input_handler->get_input());

                        m_task_db->update_object(task);

                        m_printer->msg_print("Db updated successfully!");
                        tdu::sleep(2000);
                    });

                const auto edit_content = std::make_shared<Page>(
                    "Edit content", false, [this, &task] {
                        tdu::clear_term();

                        m_printer->msg_print("content: ");
                        task.set_content(m_input_handler->get_input());

                        m_task_db->update_object(task);

                        m_printer->msg_print("Db updated successfully!");
                        tdu::sleep(2000);
                    });

                const auto change_deadline = std::make_shared<Page>(
                    "Change deadline", false, [this, &task] {
                        TimePoint deadline;

                        while (true) {
                            tdu::clear_term();

                            m_printer->msg_print("deadline: ");
                            m_printer->msg_print(
                                "deadline [YYYY.MM.DD-hh:mm]: ");
                            if (const auto datetime = tdu::parse_datetime(
                                    m_input_handler->get_input());
                                datetime.has_value()) {
                                deadline = datetime.value();
                                break;
                            } else {
                                invalid_option_event();
                            }
                        }

                        task.set_deadline(deadline);
                        m_task_db->update_object(task);

                        m_printer->msg_print("Db updated successfully!");
                        tdu::sleep(2000);
                    });

                const auto change_executor = std::make_shared<Page>(
                    "Change executor", false, [this, &task] {
                        const auto users = m_user_db->get_all_objects();

                        while (true) {
                            tdu::clear_term();

                            m_printer->msg_print("assign executor:\n");

                            for (size_t count = 0; count < users.size();
                                 ++count) {
                                m_printer->msg_print(
                                    fmt::format("[{}] {} <{}>\n", count + 1,
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

                        m_printer->msg_print("Db updated successfully!");
                        tdu::sleep(2000);
                    });

                const auto delete_task =
                    std::make_shared<Page>("Delete task", false, [this, &task] {
                        m_printer->msg_print("Are you 100% sure ? [y/n]\n");
                        const auto confirmation = m_input_handler->get_input();
                        if (confirmation == YES) {
                            m_task_db->delete_object(task.id());
                        } else if (confirmation == NO) {
                            return;
                        } else {
                            invalid_option_event();
                            return;
                        }

                        m_printer->msg_print("Db updated successfully!");
                        tdu::sleep(2000);
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

        Menu{tasks_page, m_printer, m_input_handler}.run(QUIT_OPTION);
    }

    bool user_update_event(const UpdateEvent kind, tdc::User& user) const;
    String username_validation_event() const;
    String password_validation_event() const;
    tdc::Role role_choosing_event() const;
    bool privileges_validation_event() const;
    bool privileges_validation_event(const tdc::User& user) const;
    void invalid_option_event() const;

    std::shared_ptr<tdc::User> get_current_user() const {
        return m_current_user;
    }
};

struct Updated{};
}  // namespace twodo