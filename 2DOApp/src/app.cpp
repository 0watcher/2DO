#include "2DOApp/app.hpp"

#include <fmt/color.h>
#include <fmt/core.h>
#include <memory>
#include <string>

#include <2DOApp/term.hpp>
#include <2DOCore/user.hpp>
#include <Utils/type.hpp>
#include <Utils/util.hpp>
#include "2DOCore/task.hpp"

namespace twodo {
void App::run() {
    // tdu::create_simple_app_env("2DO", {DB_NAME, ERR_LOGS_FILE_NAME,
    // USER_LOGS_FILE_NAME});
    m_current_user = std::make_shared<tdc::User>(
        tdc::User{1, "dupa", tdc::Role::Admin, "gowno"});
    m_user_db->add_object(*m_current_user);
    load_menu().run(QUIT_OPTION);
}

Menu App::load_menu() {
    const auto main = load_main_menu();
    Menu menu{main, m_printer, m_input_handler};

    main->attach(FIRST_OPTION, load_tasks_menu());
    main->attach(SECOND_OPTION, load_settings_menu());

    return std::move(menu);
}

std::shared_ptr<Page> App::load_main_menu() const {
    const auto main = std::make_shared<Page>([this] {
        m_printer->msg_print(fmt::format(
            "{}\n"
            "[1] Tasks\n"
            "[2] Setting\n"
            "[0] Exit\n"
            "-> ",
            fmt::format(fmt::bg(fmt::color::dark_golden_rod), "|2DO|")));
    });
    return std::move(main);
}

std::shared_ptr<Page> App::load_tasks_menu() const {
    const auto tasks = std::make_shared<Page>([this] {
        m_printer->msg_print(
            "Tasks:\n"
            "[1] Your tasks\n"
            "[2] Delegated tasks\n"
            "[3] Create task\n"
            "[0] Back\n"
            "-> ");
    });

    tasks->attach(FIRST_OPTION, load_your_tasks_menu());
    tasks->attach(SECOND_OPTION, load_delegated_tasks_menu());
    tasks->attach(THIRD_OPTION, load_create_tasks_menu());

    return std::move(tasks);
}

std::shared_ptr<Page> App::load_your_tasks_menu() const {
    const auto your_tasks = std::make_shared<Page>(false, [this] {
        auto tasks = m_task_db->get_all_objects<tdc::TaskDb::IdType::Executor>(
            m_current_user->id());

        const auto tasks_page = std::make_shared<Page>([this, &tasks] {
            m_printer->msg_print("Your tasks:\n");

            unsigned int count = 0;
            for (const auto& task : tasks) {
                const auto owner = m_user_db->get_object(task.owner_id());
                m_printer->msg_print(fmt::format(
                    "[{}] {} from {} ({})\n", ++count, task.topic(),
                    owner.username(),
                    (task.is_done()
                         ? fmt::format(fmt::fg(fmt::color::green), "DONE")
                         : fmt::format(fmt::fg(fmt::color::red),
                                       "INCOMPLETE"))));
            }
            m_printer->msg_print("\n-> ");
        });

        unsigned int count = 0;
        for (auto& task : tasks) {
            const auto chosen_task = std::make_shared<Page>([this, &task] {
                m_printer->msg_print(fmt::format(
                    "Topic: {}\nContent: {}\nStart Date: {}\nDeadline: "
                    "{}\nStatus: {}\n",
                    task.topic(), task.content(), task.start_date<String>(),
                    task.deadline<String>(),
                    (task.is_done()
                         ? fmt::format(fmt::fg(fmt::color::green), "DONE")
                         : fmt::format(fmt::fg(fmt::color::red),
                                       "INCOMPLETE"))));

                m_printer->msg_print(
                    "Update user:\n"
                    "[1] Mark as complete\n"
                    "[2] Discussion\n"
                    "[0] Back\n"
                    "-> ");
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

            chosen_task->attach(FIRST_OPTION, change_status);
            chosen_task->attach(SECOND_OPTION, discussion);

            tasks_page->attach(std::to_string(++count), chosen_task);
        }

        auto your_tasks_menu = Menu{tasks_page, m_printer, m_input_handler};

        your_tasks_menu.run(QUIT_OPTION);
    });

    return std::move(your_tasks);
}

std::shared_ptr<Page> App::load_delegated_tasks_menu() const {
    const auto delegated_tasks = std::make_shared<Page>([this] {
        m_printer->msg_print("Delegated tasks:\n");
        auto tasks = m_task_db->get_all_objects<tdc::TaskDb::IdType::Owner>(
            m_current_user->id());

        unsigned int count = 0;
        for (const auto& task : tasks) {
            const auto owner = m_user_db->get_object(task.owner_id());
            m_printer->msg_print(fmt::format(
                "[{}] {} from {} ({})\n", ++count, task.topic(),
                owner.username(),
                (task.is_done()
                     ? fmt::format(fmt::fg(fmt::color::green), "DONE")
                     : fmt::format(fmt::fg(fmt::color::red), "INCOMPLETE"))));
        }

        m_printer->msg_print("\n-> ");
    });

    return std::move(delegated_tasks);
}

std::shared_ptr<Page> App::load_create_tasks_menu() const {
    const auto create_task = std::make_shared<Page>(false, [this] {
        const auto task_input = [this] -> tdc::Task {
            tdu::clear_term();

            String topic, content;
            TimePoint start_date, deadline;
            unsigned int executor_id;

            m_printer->msg_print("topic: ");
            topic = m_input_handler->get_input();
            tdu::clear_term();

            m_printer->msg_print("content: ");
            content = m_input_handler->get_input();
            tdu::clear_term();

            while (true) {
                tdu::clear_term();

                m_printer->msg_print("start_date: ");
                try {
                    start_date = tdu::stotp(m_input_handler->get_input());
                    break;
                } catch (...) {
                    invalid_option_event();
                }
            }

            while (true) {
                tdu::clear_term();

                m_printer->msg_print("deadline: ");
                try {
                    deadline = tdu::stotp(m_input_handler->get_input());
                    break;
                } catch (...) {
                    invalid_option_event();
                }
            }

            const auto users = m_user_db->get_all_objects();

            while (true) {
                tdu::clear_term();
                m_printer->msg_print("assign executor:\n");
                unsigned int count = 0;
                for (const auto& user : users) {
                    m_printer->msg_print(fmt::format("[{}] {} <{}>\n", ++count,
                                                     user.username(),
                                                     user.role<String>()));
                }
                m_printer->msg_print("-> ");

                const auto executor = m_input_handler->get_input();
                if (unsigned const int executor_number = std::stoi(executor);
                    executor_number > 0 && executor_number <= count) {
                    executor_id = users[executor_number - 1].id();
                    break;
                }

                invalid_option_event();
            }

            return tdc::Task{topic,    content,     start_date,
                             deadline, executor_id, m_current_user->id(),
                             false};
        };

        m_task_db->add_object(task_input());
        m_printer->msg_print("Task has been added successfully!");
    });

    return std::move(create_task);
}

std::shared_ptr<Page> App::load_settings_menu() {
    const auto settings = std::make_shared<Page>([this] {
        m_printer->msg_print(
            "Settings:\n"
            "[1] User Manager\n"
            "[2] Advanced\n"
            "[0] Back\n"
            "-> ");
    });

    settings->attach(FIRST_OPTION, load_user_manager_menu());
    settings->attach(SECOND_OPTION, load_advanced_menu());

    return std::move(settings);
}

std::shared_ptr<Page> App::load_user_manager_menu() {
    const auto user_manager = std::make_shared<Page>([this] {
        m_printer->msg_print(
            "User Manager:\n"
            "[1] Manage users\n"
            "[2] Create new user\n"
            "[0] Back\n"
            "-> ");
    });

    user_manager->attach(FIRST_OPTION, load_user_update_menu());
    user_manager->attach(SECOND_OPTION, load_new_user_menu());

    return std::move(user_manager);
}

std::shared_ptr<Page> App::load_user_update_menu() {
    const auto edit_users = std::make_shared<Page>(false, [this] {
        bool should_collapse = false;

        auto users = m_user_db->get_all_objects();

        const auto root_page = std::make_shared<Page>([this, &users] {
            m_printer->msg_print("Users:\n");

            unsigned int count = 0;
            for (const auto& user : users) {
                m_printer->msg_print(fmt::format("[{}] {} <{}>\n", ++count,
                                                 user.username(),
                                                 user.role<String>()));
            }
            m_printer->msg_print(
                "[0] Back\n"
                "-> ");
        });

        unsigned int count = 0;
        for (auto& user : users) {
            const auto chosen_user =
                std::make_shared<Page>([this, &should_collapse] {
                    m_printer->msg_print(
                        "Update user:\n"
                        "[1] Change username\n"
                        "[2] Change password\n"
                        "[3] Change role\n"
                        "[4] Delete user\n"
                        "[0] Back\n"
                        "-> ");
                });

            const auto username_update =
                std::make_shared<Page>(false, [this, &user] {
                    if (!privileges_validation_event(user))
                        return;
                    user_update_event(UpdateEvent::UsernameUpdate, user);
                });

            const auto password_update =
                std::make_shared<Page>(false, [this, &user] {
                    if (!privileges_validation_event(user))
                        return;
                    user_update_event(UpdateEvent::PasswordUpdate, user);
                });

            const auto role_update =
                std::make_shared<Page>(false, [this, &user] {
                    if (!privileges_validation_event(user))
                        return;
                    user_update_event(UpdateEvent::RoleUpdate, user);
                });

            const auto user_deletion =
                std::make_shared<Page>(false, [this, &user, &should_collapse] {
                    if (!privileges_validation_event(user))
                        return;
                    if (user_update_event(UpdateEvent::UserDelete, user)) {
                        should_collapse = true;
                    }
                });

            chosen_user->attach(FIRST_OPTION, username_update);
            chosen_user->attach(SECOND_OPTION, password_update);
            chosen_user->attach(THIRD_OPTION, role_update);
            chosen_user->attach(FOURTH_OPTION, user_deletion);

            root_page->attach(std::to_string(++count), chosen_user);
        }

        auto users_menu = Menu{root_page, m_printer, m_input_handler};

        users_menu.run(QUIT_OPTION);
    });

    return std::move(edit_users);
}

std::shared_ptr<Page> App::load_new_user_menu() {
    const auto add_new_user = std::make_shared<Page>(false, [this] {
        if (m_current_user->role<tdc::Role>() == tdc::Role::Admin) {
            const String username = username_validation_event();
            const String password = password_validation_event();
            if (password.empty())
                return;

            tdc::Role role;
            while (true) {
                tdu::clear_term();
                m_printer->msg_print("Role:\n[1] Admin\n[2] User\n-> ");
                String str_role = m_input_handler->get_input();
                if (str_role == FIRST_OPTION) {
                    role = tdc::Role::Admin;
                    break;
                } else if (str_role == SECOND_OPTION) {
                    role = tdc::Role::User;
                    break;
                } else {
                    invalid_option_event();
                }
            }

            m_user_db->add_object(tdc::User{username, role, password});

            m_printer->msg_print("User has been added successfully!");
        }
    });

    return std::move(add_new_user);
}

std::shared_ptr<Page> App::load_advanced_menu() {
    const auto advanced = std::make_shared<Page>(
        [] { fmt::print("Advanced:\n[1] Wipe all data\n-> "); });

    const auto wipe_all_data = std::make_shared<Page>(false, [this] {
        if (!privileges_validation_event())
            return;

        m_printer->msg_print("Are you 100% sure? [y/n]\n-> ");

        if (const auto choice = m_input_handler->get_input(); choice == YES) {
            tdu::wipe_simple_app_env("2DO");
            tdu::create_simple_app_env("2DO", {DB_NAME, ERR_LOGS_FILE_NAME});
            m_printer->msg_print("Data wiped!");
        } else if (choice == NO) {
            return;
        } else {
            invalid_option_event();
        }
    });

    advanced->attach(FIRST_OPTION, wipe_all_data);

    return std::move(advanced);
}

bool App::user_update_event(UpdateEvent kind, tdc::User& user) {
    tdu::clear_term();
    switch (kind) {
        case UpdateEvent::UsernameUpdate: {
            String username{};
            do {
                m_printer->msg_print("new username: ");
                username = m_input_handler->get_input();
            } while (!m_amanager.username_validation(username));
            user.set_username(username);
            m_user_db->update_object(user);
        } break;

        case UpdateEvent::PasswordUpdate: {
            String password{};
            do {
                m_printer->msg_print("new password: ");
                password = m_input_handler->get_secret();
            } while (!m_amanager.password_validation(password));
            user.set_password(password);
            m_user_db->update_object(user);

        } break;

        case UpdateEvent::RoleUpdate: {
            tdc::Role role;
            while (true) {
                m_printer->msg_print("Role:\n[1] Admin\n[2] User\n-> ");
                String str_role = m_input_handler->get_input();
                if (str_role == FIRST_OPTION) {
                    role = tdc::Role::Admin;
                    break;
                } else if (str_role == SECOND_OPTION) {
                    role = tdc::Role::User;
                    break;
                } else {
                    invalid_option_event();
                }
            }
            user.set_role(role);
            m_user_db->update_object(user);
        } break;

        case UpdateEvent::UserDelete: {
            m_printer->msg_print("Are you 100% sure ? [y/n]\n");
            const auto confirmation = m_input_handler->get_input();
            if (confirmation == YES) {
                m_user_db->delete_object(user.id());
                return true;
            } else if (confirmation == NO) {
                return false;
            } else {
                invalid_option_event();
            }
        } break;
    }

    m_printer->msg_print("Db updated successfully!");
    tdu::sleep(2000);
};

String App::username_validation_event() {
    tdu::clear_term();
    String username;

    while (true) {
        tdu::clear_term();

        m_printer->msg_print("username: ");
        username = m_input_handler->get_input();

        const auto result = m_amanager.username_validation(username);
        if (result) {
            return username;
        }

        switch (result.err()) {
            case tdc::AuthErr::InvalidNameLength:
                m_printer->err_print("Name too short!");
                break;
            case tdc::AuthErr::AlreadyExistingName:
                m_printer->err_print("Name already exists!");
                break;
        }

        tdu::sleep(2000);
    }
}

String App::password_validation_event() {
    String password;

    unsigned short tries = 0;
    while (true) {
        tdu::clear_term();

        m_printer->msg_print("password: ");
        password = m_input_handler->get_secret();

        const auto result = m_amanager.password_validation(password);
        if (result) {
            return password;
        }

        switch (result.err()) {
            case tdc::AuthErr::InvalidPassLength:
                m_printer->err_print(
                    "\nPassword must contain at "
                    "least 8 characters and max "
                    "20!");
                break;
            case tdc::AuthErr::MissingUpperCase:
                m_printer->err_print(
                    "\nPassword must contain at "
                    "least one uppercase "
                    "letter!");
                break;
            case tdc::AuthErr::MissingLowerCase:
                m_printer->err_print(
                    "\nPassword must contain at "
                    "least one lowercase "
                    "letter!");
                break;
            case tdc::AuthErr::MissingNumber:
                m_printer->err_print(
                    "\nPassword must contain at "
                    "least one number !");
                break;
            case tdc::AuthErr::MissingSpecialCharacter:
                m_printer->err_print(
                    "\nPassword must contain at "
                    "least one special "
                    "character!");
                break;
        }

        if (tries >= 3) {
            m_printer->err_print("\nAll tries exhausted!");
            tdu::sleep(2000);
            return "";
        }

        tdu::sleep(2000);

        ++tries;
    }
}

bool App::privileges_validation_event(const tdc::User& user) const {
    if (m_current_user->role<tdc::Role>() != tdc::Role::Admin &&
        m_current_user->id() != user.id()) {
        m_printer->err_print("You're not allowed to do this!");
        tdu::sleep(2000);
        return false;
    }

    return true;
};

bool App::privileges_validation_event() const {
    if (m_current_user->role<tdc::Role>() != tdc::Role::Admin) {
        m_printer->err_print("You're not allowed to do this!");
        tdu::sleep(2000);
        return false;
    }

    return true;
};

void App::invalid_option_event() const {
    m_printer->err_print("Invalid option!");
    tdu::sleep(2000);
    tdu::clear_term();
};
}  // namespace twodo
