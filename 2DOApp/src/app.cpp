#include "2DOApp/app.hpp"

#include <filesystem>
#include <memory>
#include <thread>

#include "Utils/util.hpp"

#include <fmt/color.h>
#include <fmt/core.h>

namespace twodo {
App::App() {
    const auto base_path = tdu::create_app_env(
        ENV_FOLDER_NAME, {DB_NAME, ERR_LOGS_FILE_NAME, USER_LOGS_FILE_NAME});

    m_user_db = std::make_shared<tdc::UserDb>(base_path / DB_NAME);
    m_task_db = tdc::TaskDb{base_path / DB_NAME};
    m_message_db = tdc::MessageDb{base_path / DB_NAME};
    m_auth_manager = tdc::AuthenticationManager{m_user_db};
};

void App::run() {
    if (is_first_user()) {
        sing_up();
    }

    while (sing_in()) {
        try {
            load_menu().run(QUIT_OPTION);
        } catch (const Wiped&) {
            sing_up();
        }
    }
}

tdc::Menu App::load_menu() {
    const auto main = std::make_shared<tdc::Page>(
        fmt::format("2DO [{}]", m_current_user->username()));

    main->attach(FIRST_OPTION, load_tasks_menu());
    main->attach(SECOND_OPTION, load_settings_menu());

    return tdc::Menu{main, m_printer, m_input_handler};
}

std::shared_ptr<tdc::Page> App::load_tasks_menu() const {
    const auto tasks = std::make_shared<tdc::Page>("Tasks");

    tasks->attach(FIRST_OPTION,
                  std::make_shared<tdc::Page>("Your Tasks", false, [&] {
                      load_update_tasks_menu<tdc::TaskDb::IdType::Executor>();
                  }));

    tasks->attach(SECOND_OPTION,
                  std::make_shared<tdc::Page>("Delegated Tasks", false, [&] {
                      load_update_tasks_menu<tdc::TaskDb::IdType::Owner>();
                  }));

    tasks->attach(THIRD_OPTION, load_create_tasks_menu());

    return std::move(tasks);
}

std::shared_ptr<tdc::Page> App::load_create_tasks_menu() const {
    return std::make_shared<tdc::Page>("Create Task", false, [&] {
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

std::shared_ptr<tdc::Page> App::load_settings_menu() {
    const auto settings = std::make_shared<tdc::Page>("Settings");

    settings->attach(FIRST_OPTION, load_user_manager_menu());
    settings->attach(SECOND_OPTION, load_advanced_menu());

    return std::move(settings);
}

std::shared_ptr<tdc::Page> App::load_user_manager_menu() {
    const auto user_manager = std::make_shared<tdc::Page>("User Manager");

    user_manager->attach(FIRST_OPTION, load_user_update_menu());
    user_manager->attach(SECOND_OPTION, load_new_user_menu());

    return std::move(user_manager);
}

std::shared_ptr<tdc::Page> App::load_user_update_menu() {
    return std::make_shared<tdc::Page>("Manage Users", false, [&] {
        const auto root_page = std::make_shared<tdc::Page>("Users");

        Vector<tdc::User> users = m_user_db->get_all_objects();

        unsigned int count = 0;
        for (auto& user : users) {
            const auto chosen_user = std::make_shared<tdc::Page>(
                fmt::format("{} <{}>", user.username(), user.role<String>()));

            const auto username_update =
                std::make_shared<tdc::Page>("Change Username", false, [&] {
                    if (!privileges_validation_event(user)) {
                        return;
                    }
                    if (user_update_event(UserUpdateEvent::UsernameUpdate,
                                          user)) {
                        throw Updated{};  // ugly i know ...
                    }
                });

            const auto password_update =
                std::make_shared<tdc::Page>("Change Password", false, [&] {
                    if (!privileges_validation_event(user)) {
                        return;
                    }
                    if (user_update_event(UserUpdateEvent::PasswordUpdate,
                                          user)) {
                        throw Updated{};
                    }
                });

            const auto role_update =
                std::make_shared<tdc::Page>("Change Role", false, [&] {
                    if (!privileges_validation_event(user)) {
                        return;
                    }
                    if (user_update_event(UserUpdateEvent::RoleUpdate, user)) {
                        throw Updated{};
                    }
                });

            const auto user_deletion =
                std::make_shared<tdc::Page>("Delete User", false, [&] {
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
            tdc::Menu{root_page, m_printer, m_input_handler}.run(QUIT_OPTION);
        } catch (const Updated) {
        }
    });
}

std::shared_ptr<tdc::Page> App::load_new_user_menu() const {
    return std::make_shared<tdc::Page>("Create User", false, [&] {
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

std::shared_ptr<tdc::Page> App::load_advanced_menu() const {
    const auto advanced = std::make_shared<tdc::Page>("Advanced");

    const auto wipe_all_data =
        std::make_shared<tdc::Page>("Wipe All Data", false, [&] {
            if (!privileges_validation_event())
                return;

            m_printer->msg_print("Are you 100% sure? [y/n]\n-> ");

            if (const auto choice = m_input_handler->get_input();
                choice == YES) {
                tdc::clear_all_db_data(
                    fs::current_path().root_path() / "2DO" / DB_NAME,
                    {"users", "tasks", "messages"});
                m_printer->msg_print("Data wiped!");
                tdu::sleep(2000);

                throw Wiped{};
            } else if (choice == NO) {
                return;
            } else {
                invalid_option_event();
            }
        });

    advanced->attach(FIRST_OPTION, wipe_all_data);

    return std::move(advanced);
}

bool App::user_update_event(UserUpdateEvent kind, tdc::User& user) {
    tdu::clear_term();
    switch (kind) {
        case UserUpdateEvent::UsernameUpdate: {
            user.set_username(username_validation_event());
            m_user_db->update_object(user);
            update_current_user(user);

            m_printer->msg_print("Db updated successfully!");
            tdu::sleep(2000);
            return true;
        } break;

        case UserUpdateEvent::PasswordUpdate: {
            user.set_password(password_validation_event());
            m_user_db->update_object(user);
            update_current_user(user);

            m_printer->msg_print("Db updated successfully!");
            tdu::sleep(2000);
            return true;
        } break;

        case UserUpdateEvent::RoleUpdate: {
            if (!privileges_validation_event()) {
                return false;
            }

            const tdc::Role role = role_choosing_event();
            user.set_role(role);
            m_user_db->update_object(user);
            update_current_user(user);

            m_printer->msg_print("Db updated successfully!");
            tdu::sleep(2000);
            return true;
        } break;

        case UserUpdateEvent::UserDelete: {
            m_printer->msg_print("Are you 100% sure ? [y/n]\n");
            const auto confirmation = m_input_handler->get_input();
            if (confirmation == YES) {
                if (user == m_current_user) {
                    m_printer->err_print(
                        "You can't delete yourself! If you want to delete "
                        "yourself then try wipe all data!");
                    tdu::sleep(2000);
                    return false;
                }

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

void App::update_current_user(const tdc::User& user) {
    if (user == m_current_user) {
        m_current_user = user;
    }
}

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

void App::discussion_event(const tdc::Task& task) const {
    std::atomic<bool> should_close(false);
    std::atomic<unsigned int> last_msg_id(0);

    auto receive_msg = [&]() {
        while (!should_close) {
            const auto new_msg = m_message_db->get_newest_object();

            if (new_msg.has_value() &&
                new_msg.value().message_id() > last_msg_id) {
                tdu::clear_term();

                last_msg_id = new_msg.value().message_id();

                const auto messages = m_message_db->get_all_objects(task.id());

                for (const auto& message : messages) {
                    m_printer->msg_print(fmt::format(
                        "[{}] <{}>: {}\n",
                        tdu::to_string(message.timestamp<TimePoint>()),
                        message.sender_name(), message.content()));
                }

                m_printer->msg_print(
                    fmt::format("<{}>: ", m_current_user->username()));
            }

            tdu::sleep(100);
        }
    };

    auto send_msg = [&]() {
        while (!should_close) {
            std::string sent_message = m_input_handler->get_input();
            if (sent_message == "0") {
                should_close = true;
                break;
            }

            m_message_db->add_object(
                tdc::Message{task.id(), m_current_user->username(),
                             sent_message, tdu::get_current_timestamp()});
        }
    };

    std::thread receive_thread = std::thread(receive_msg);
    std::thread send_thread = std::thread(send_msg);

    receive_thread.join();
    send_thread.join();
}

String App::username_validation_event() const {
    tdu::clear_term();
    String username;

    while (true) {
        tdu::clear_term();

        m_printer->msg_print("Username: ");
        username = m_input_handler->get_input();

        const auto result = m_auth_manager->username_validation(username);
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

    while (true) {
        tdu::clear_term();

        m_printer->msg_print("Password: ");
        password = m_input_handler->get_secret();

        const auto result = m_auth_manager->password_validation(password);
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

        tdu::sleep(2000);
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

        m_printer->msg_print(fmt::format("{} [YYYY-MM-DD hh:mm]: ", msg));

        if (const auto datetime =
                tdu::to_time_point(m_input_handler->get_input());
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

bool App::sing_in() {
    tdu::clear_term();

    while (true) {
        tdu::clear_term();

        m_printer->msg_print("Username: ");
        const auto username = m_input_handler->get_input();
        if (username == "0") {
            return false;
        }

        if (const auto user = m_user_db->find_object_by_unique_column(username);
            user) {
            while (true) {
                tdu::clear_term();

                m_printer->msg_print("Password: ");
                if (const auto password = m_input_handler->get_secret();
                    tdu::hash(password) == user.value().password()) {
                    m_current_user = user.value();

                    tdu::log_to_file(fmt::format("{}", user.value().username()),
                                     fs::current_path().root_path() /
                                         ENV_FOLDER_NAME / USER_LOGS_FILE_NAME);

                    m_printer->msg_print("\nSuccessfully logged in!");
                    tdu::sleep(2000);
                    return true;
                    break;
                }

                m_printer->err_print("\nInvalid Password!");
                tdu::sleep(2000);
            }
            break;
        }

        m_printer->err_print("\nUser with that name does not exist!");
        tdu::sleep(2000);
    }
}

void App::sing_up() const {
    tdu::clear_term();

    m_printer->msg_print("First Sing Up");
    tdu::sleep(2000);
    tdu::clear_term();

    const String username = username_validation_event();
    const String password = password_validation_event();

    if (is_first_user()) {
        m_user_db->add_object(tdc::User{username, tdc::Role::Admin, password});
    } else {
        m_user_db->add_object(tdc::User{username, tdc::Role::User, password});
    }

    m_printer->msg_print("\nSuccessfully added user!");
    tdu::sleep(2000);
}

bool App::is_first_user() const {
    return m_user_db->is_table_empty();
}

bool App::is_task_accessible(const tdc::Task& task) const {
    return !task.is_done() &&
               task.deadline<TimePoint>() > tdu::get_current_timestamp() ||
           m_current_user->id() == task.owner_id();
}
}  // namespace twodo
