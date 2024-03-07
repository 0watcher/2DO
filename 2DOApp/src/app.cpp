#include "2DOApp/app.hpp"

#include <fmt/color.h>
#include <fmt/core.h>
#include "2DOCore/user.hpp"
#include "Utils/type.hpp"
#include "Utils/util.hpp"

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
    const auto main = std::make_shared<Page>("2DO");

    main->attach(FIRST_OPTION, load_tasks_menu());
    main->attach(SECOND_OPTION, load_settings_menu());

    return Menu{main, m_printer, m_input_handler};
}

std::shared_ptr<Page> App::load_tasks_menu() const {
    const auto tasks = std::make_shared<Page>("Tasks");

    tasks->attach(FIRST_OPTION,
                  std::make_shared<Page>("Your Tasks", false, [this] {
                      load_update_tasks_menu<tdc::TaskDb::IdType::Executor>();
                  }));

    tasks->attach(SECOND_OPTION,
                  std::make_shared<Page>("Delegated Tasks", false, [this] {
                      load_update_tasks_menu<tdc::TaskDb::IdType::Owner>();
                  }));

    tasks->attach(THIRD_OPTION, load_create_tasks_menu());

    return std::move(tasks);
}

std::shared_ptr<Page> App::load_create_tasks_menu() const {
    return std::make_shared<Page>("Create Task", false, [this] {
        const auto task_input = [this] -> tdc::Task {
            tdu::clear_term();

            const auto topic = string_input("Topic: ");
            tdu::clear_term();

            const auto content = string_input("Content: ");
            tdu::clear_term();

            const TimePoint start_date =
                datetime_validation_event("Start Date");

            const TimePoint deadline = datetime_validation_event("Deadline");

            const auto users = m_user_db->get_all_objects();

            const unsigned int executor_id = executor_choosing_event().id();

            return tdc::Task{topic,    content,     start_date,
                             deadline, executor_id, m_current_user->id(),
                             false};
        };

        m_task_db->add_object(task_input());
        m_printer->msg_print("Task has been added successfully!");
    });
}

std::shared_ptr<Page> App::load_settings_menu() const {
    const auto settings = std::make_shared<Page>("Settings");

    settings->attach(FIRST_OPTION, load_user_manager_menu());
    settings->attach(SECOND_OPTION, load_advanced_menu());

    return std::move(settings);
}

std::shared_ptr<Page> App::load_user_manager_menu() const {
    const auto user_manager = std::make_shared<Page>("User Manager");

    user_manager->attach(FIRST_OPTION, load_user_update_menu());
    user_manager->attach(SECOND_OPTION, load_new_user_menu());

    return std::move(user_manager);
}

std::shared_ptr<Page> App::load_user_update_menu() const {
    return std::make_shared<Page>("Manage Users", false, [this] {
        const auto root_page = std::make_shared<Page>("Users");

        Vector<tdc::User> users = m_user_db->get_all_objects();

        unsigned int count = 0;
        for (auto& user : users) {
            const auto chosen_user = std::make_shared<Page>(
                fmt::format("{} <{}>", user.username(), user.role<String>()));

            const auto username_update =
                std::make_shared<Page>("Change Username", false, [this, &user] {
                    if (!privileges_validation_event(user)) {
                        return;
                    }
                    if (user_update_event(UserUpdateEvent::UsernameUpdate,
                                          user)) {
                        throw Updated{};  // ugly i know ...
                    }
                });

            const auto password_update =
                std::make_shared<Page>("Change Password", false, [this, &user] {
                    if (!privileges_validation_event(user)) {
                        return;
                    }
                    if (user_update_event(UserUpdateEvent::PasswordUpdate,
                                          user)) {
                        throw Updated{};
                    }
                });

            const auto role_update =
                std::make_shared<Page>("Change Role", false, [this, &user] {
                    if (!privileges_validation_event(user)) {
                        return;
                    }
                    if (user_update_event(UserUpdateEvent::RoleUpdate, user)) {
                        throw Updated{};
                    }
                });

            const auto user_deletion =
                std::make_shared<Page>("Delete User", false, [this, &user] {
                    if (!privileges_validation_event(user)) {
                        return;
                    }
                    if (user_update_event(UserUpdateEvent::UserDelete, user)) {
                        throw Updated{};
                    }
                });

            chosen_user->attach(FIRST_OPTION, username_update);
            chosen_user->attach(SECOND_OPTION, password_update);
            chosen_user->attach(THIRD_OPTION, role_update);
            chosen_user->attach(FOURTH_OPTION, user_deletion);

            root_page->attach(std::to_string(++count), chosen_user);
        }

        try {
            Menu{root_page, m_printer, m_input_handler}.run(QUIT_OPTION);
        } catch (const Updated) {
        }
    });
}

std::shared_ptr<Page> App::load_new_user_menu() const {
    return std::make_shared<Page>("Create User", false, [this] {
        if (m_current_user->role<tdc::Role>() == tdc::Role::Admin) {
            const String username = username_validation_event();
            const String password = password_validation_event();
            if (password.empty()) {
                return;
            }
            const tdc::Role role = role_choosing_event();

            m_user_db->add_object(tdc::User{username, role, password});

            m_printer->msg_print("User has been added successfully!");
        }
    });
}

std::shared_ptr<Page> App::load_advanced_menu() const {
    const auto advanced = std::make_shared<Page>("Advanced");

    const auto wipe_all_data = std::make_shared<
        Page>("Wipe All Data", false, [this] {
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

bool App::user_update_event(UserUpdateEvent kind, tdc::User& user) const {
    tdu::clear_term();
    switch (kind) {
        case UserUpdateEvent::UsernameUpdate: {
            String username{};
            do {
                m_printer->msg_print("New Username: ");
                username = m_input_handler->get_input();
            } while (!m_auth_manager.username_validation(username));

            user.set_username(username);
            m_user_db->update_object(user);

            m_printer->msg_print("Db updated successfully!");
            tdu::sleep(2000);
            return true;
        } break;

        case UserUpdateEvent::PasswordUpdate: {
            String password{};
            do {
                m_printer->msg_print("New Password: ");
                password = m_input_handler->get_secret();
            } while (!m_auth_manager.password_validation(password));

            user.set_password(password);
            m_user_db->update_object(user);

            m_printer->msg_print("Db updated successfully!");
            tdu::sleep(2000);
            return true;
        } break;

        case UserUpdateEvent::RoleUpdate: {
            const tdc::Role role = role_choosing_event();
            user.set_role(role);
            m_user_db->update_object(user);

            m_printer->msg_print("Db updated successfully!");
            tdu::sleep(2000);
            return true;
        } break;

        case UserUpdateEvent::UserDelete: {
            m_printer->msg_print("Are you 100% sure ? [y/n]\n");
            const auto confirmation = m_input_handler->get_input();
            if (confirmation == YES) {
                m_user_db->delete_object(user.id());

                m_printer->msg_print("Db updated successfully!");
                tdu::sleep(2000);
                return true;
            } else if (confirmation == NO) {
                return false;
            } else {
                invalid_option_event();
                return false;
            }
        } break;
    }
};

bool App::task_update_event(const TaskUpdateEvent kind, tdc::Task& task) const {
    tdu::clear_term();

    switch (kind) {
        case TaskUpdateEvent::TopicUpdate: {
            tdu::clear_term();

            task.set_topic(string_input("Topic: "));

            m_task_db->update_object(task);

            m_printer->msg_print("Db updated successfully!");
            tdu::sleep(2000);
            return true;
        } break;
        case TaskUpdateEvent::ContentUpdate: {
            tdu::clear_term();

            task.set_content(string_input("Content: "));

            m_task_db->update_object(task);

            m_printer->msg_print("Db updated successfully!");
            tdu::sleep(2000);
            return true;
        } break;
        case TaskUpdateEvent::DeadlineUpdate: {
            task.set_deadline(datetime_validation_event("Deadline: "));
            m_task_db->update_object(task);

            m_printer->msg_print("Db updated successfully!");
            tdu::sleep(2000);
            return true;
        } break;
        case TaskUpdateEvent::ExecutorUpdate: {
            task.set_executor(executor_choosing_event().id());
            m_task_db->update_object(task);

            m_printer->msg_print("Db updated successfully!");
            tdu::sleep(2000);
            return true;
        } break;
        case TaskUpdateEvent::TaskDelete: {
            m_printer->msg_print("Are you 100% sure ? [y/n]\n");
            const auto confirmation = m_input_handler->get_input();
            if (confirmation == YES) {
                m_task_db->delete_object(task.id());
            } else if (confirmation == NO) {
                return false;
            } else {
                invalid_option_event();
                return false;
            }

            m_printer->msg_print("Db updated successfully!");
            tdu::sleep(2000);
            return true;
        } break;
    }
}

bool App::task_completion_event(tdc::Task& task) const {
    tdu::clear_term();

    m_printer->msg_print("Are you 100% sure ? [y/n]\n");
    const auto choice = m_input_handler->get_input();

    if (choice == "y") {
        task.set_is_done(true);
        m_task_db->update_object(task);

        m_printer->msg_print("Congrats you've completed the task!");
        tdu::sleep(2000);
        return true;
    } else if (choice == "n") {
        return false;
    } else {
        invalid_option_event();
        return false;
    }
}

void App::discussion_event(tdc::Task& task) const {
    Vector<tdc::Message> messages{};
    bool should_close = false;

    const auto main_chat_loop = [this, &task, &messages, &should_close] {
        tdu::clear_term();

        while (true) {
            m_printer->msg_print(
                fmt::format("<{}>: ", m_current_user->username()));
            String sended_message = m_input_handler->get_input();
            if (sended_message == "0") {
                should_close = true;
                break;
            }

            const auto prepared_msg =
                tdc::Message{task.id(), m_current_user->username(),
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

    const auto check_for_msg = [this, &messages, &task, &should_close] {
        while (!should_close) {
            const auto new_msg = m_message_db->get_newest_object();

            if (messages.empty() ||
                (new_msg.message_id() > messages.back().message_id())) {
                tdu::clear_term();

                messages = m_message_db->get_all_objects(task.id());
                for (const auto& msg : messages) {
                    m_printer->msg_print(fmt::format(
                        "[{}] <{}>: {}\n",
                        tdu::format_datetime(msg.timestamp<TimePoint>()),
                        msg.sender_name(), msg.content()));
                }
            }

            tdu::sleep(100);
        }
    };

    tdu::fork_tasks(main_chat_loop, check_for_msg);
}

String App::username_validation_event() const {
    tdu::clear_term();
    String username;

    while (true) {
        tdu::clear_term();

        m_printer->msg_print("Username: ");
        username = m_input_handler->get_input();

        const auto result = m_auth_manager.username_validation(username);
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

String App::password_validation_event() const {
    String password;

    unsigned short tries = 0;
    while (true) {
        tdu::clear_term();

        m_printer->msg_print("Password: ");
        password = m_input_handler->get_secret();

        const auto result = m_auth_manager.password_validation(password);
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

tdc::Role App::role_choosing_event() const {
    while (true) {
        tdu::clear_term();
        m_printer->menu_print(
            "Role", {{FIRST_OPTION, "Admin"}, {SECOND_OPTION, "User"}});

        String str_role = m_input_handler->get_input();
        if (str_role == FIRST_OPTION) {
            return tdc::Role::Admin;
            break;
        } else if (str_role == SECOND_OPTION) {
            return tdc::Role::User;
            break;
        } else {
            invalid_option_event();
        }
    }
};

tdc::User App::executor_choosing_event() const {
    const auto users = m_user_db->get_all_objects();

    while (true) {
        tdu::clear_term();
        m_printer->msg_print("Choose Executor: \n");

        unsigned count = 0;
        for (const auto user : users) {
            m_printer->msg_print(fmt::format(
                "[{}] {} <{}>\n",
                fmt::format(fg(fmt::color::blue_violet),
                            std::to_string(++count)),
                fmt::format(fg(fmt::color::blue_violet), user.username()),
                fmt::format(fg(fmt::color::blue_violet), user.role<String>())));
        }
        m_printer->msg_print("-> ");

        const auto executor = m_input_handler->get_input();
        if (unsigned const int executor_number = std::stoi(executor);
            executor_number > 0 && executor_number <= count) {
            return users[executor_number - 1];
            break;
        }

        invalid_option_event();
    }
}

TimePoint App::datetime_validation_event(StringView msg) const {
    while (true) {
        tdu::clear_term();

        m_printer->msg_print(fmt::format("{} [YYYY.MM.DD-hh:mm]: ", msg));

        if (const auto datetime =
                tdu::parse_datetime(m_input_handler->get_input());
            datetime) {
            return datetime.value();
            break;
        } else {
            invalid_option_event();
        }
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

String App::string_input(StringView msg) const {
    m_printer->msg_print(msg);
    return m_input_handler->get_input();
}
}  // namespace twodo
