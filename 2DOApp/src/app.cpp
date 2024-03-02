#include "2DOApp/app.hpp"

#include <fmt/color.h>
#include <fmt/core.h>
#include "2DOCore/user.hpp"

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
                  std::make_shared<Page>("Your tasks", false, [this] {
                      load_update_tasks_menu<tdc::TaskDb::IdType::Executor>();
                  }));

    tasks->attach(SECOND_OPTION,
                  std::make_shared<Page>("Delegated tasks", false, [this] {
                      load_update_tasks_menu<tdc::TaskDb::IdType::Owner>();
                  }));

    tasks->attach(THIRD_OPTION, load_create_tasks_menu());

    return std::move(tasks);
}

std::shared_ptr<Page> App::load_create_tasks_menu() const {
    return std::make_shared<Page>("Create Task", false, [this] {
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

                unsigned count = 0;
                for (const auto user : users) {
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
        bool should_collapse = false;

        auto users = m_user_db->get_all_objects();

        const auto root_page = std::make_shared<Page>("Users");

        unsigned int count = 0;
        for (auto& user : users) {
            const auto chosen_user =
                std::make_shared<Page>(fmt::format("[{}] {} <{}>\n", count + 1,
                                                   users[count].username(),
                                                   users[count].role<String>()),
                                       [this, &should_collapse] {
                                           if (should_collapse)
                                               return;
                                       });

            const auto username_update =
                std::make_shared<Page>("Change username", false, [this, &user] {
                    if (!privileges_validation_event(user))
                        return;
                    user_update_event(UpdateEvent::UsernameUpdate, user);
                });

            const auto password_update =
                std::make_shared<Page>("Change password", false, [this, &user] {
                    if (!privileges_validation_event(user))
                        return;
                    user_update_event(UpdateEvent::PasswordUpdate, user);
                });

            const auto role_update =
                std::make_shared<Page>("Change role", false, [this, &user] {
                    if (!privileges_validation_event(user))
                        return;
                    user_update_event(UpdateEvent::RoleUpdate, user);
                });

            const auto user_deletion = std::make_shared<Page>(
                "Delete user", false, [this, &user, &should_collapse] {
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

        Menu{root_page, m_printer, m_input_handler}.run(QUIT_OPTION);
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

bool App::user_update_event(UpdateEvent kind, tdc::User& user) const {
    tdu::clear_term();
    switch (kind) {
        case UpdateEvent::UsernameUpdate: {
            String username{};
            do {
                m_printer->msg_print("new username: ");
                username = m_input_handler->get_input();
            } while (!m_auth_manager.username_validation(username));

            user.set_username(username);
            m_user_db->update_object(user);
        } break;

        case UpdateEvent::PasswordUpdate: {
            String password{};
            do {
                m_printer->msg_print("new password: ");
                password = m_input_handler->get_secret();
            } while (!m_auth_manager.password_validation(password));

            user.set_password(password);
            m_user_db->update_object(user);

        } break;

        case UpdateEvent::RoleUpdate: {
            const tdc::Role role = role_choosing_event();
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

String App::username_validation_event() const {
    tdu::clear_term();
    String username;

    while (true) {
        tdu::clear_term();

        m_printer->msg_print("username: ");
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

        m_printer->msg_print("password: ");
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
        m_printer->msg_print("Role:\n[1] Admin\n[2] User\n-> ");
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
