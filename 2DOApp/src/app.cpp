#include "2DOApp/app.hpp"

#include <fmt/core.h>
#include <memory>
#include <string>

#include <2DOApp/term.hpp>
#include <2DOCore/user.hpp>
#include <Utils/type.hpp>
#include <Utils/util.hpp>

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
        m_printer->msg_print(
            "Main Menu:\n"
            "[1] Tasks\n"
            "[2] Setting\n"
            "[0] Exit\n"
            "-> ");
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

    const auto your_tasks = std::make_shared<Page>([this] {
        m_printer->msg_print("Your tasks:\n");
        auto tasks = m_task_db->get_all_objects(m_current_user->id());

        unsigned int count = 1;
        for (const auto& task : tasks) {
            const auto owner = m_user_db->get_object(task.owner_id());
            m_printer->msg_print(fmt::format(
                "[{}] {} from {} ({})", count++, task.topic(), owner.username(),
                (task.is_done() ? "DONE" : "INCOMPLETE")));
        }
    });

    const auto delegated_tasks = std::make_shared<Page>([] { fmt::print(""); });

    const auto create_task = std::make_shared<Page>(false, [] {

    });

    tasks->attach(FIRST_OPTION, your_tasks);
    tasks->attach(SECOND_OPTION, delegated_tasks);
    tasks->attach(THIRD_OPTION, create_task);

    return std::move(tasks);
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

    const auto user_manager = std::make_shared<Page>([this] {
        m_printer->msg_print(
            "User Manager:\n"
            "[1] Manage users\n"
            "[2] Create new user\n"
            "[0] Back\n"
            "-> ");
    });

    const auto edit_users = std::make_shared<Page>(false, [this] {
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
            const auto chosen_user = std::make_shared<Page>([this] {
                m_printer->msg_print(
                    "Update user:\n"
                    "[1] Change username\n"
                    "[2] Change password\n"
                    "[3] Change role\n"
                    "[0] Back\n"
                    "-> ");
            });

            root_page->attach(std::to_string(++count), chosen_user);
        }

        const auto user_edit_menu =
            std::make_shared<Menu>(root_page, m_printer, m_input_handler);

        user_edit_menu->run(QUIT_OPTION);
    });

    const auto add_new_user = std::make_shared<Page>(false, [this] {
        if (m_current_user->role<tdc::Role>() == tdc::Role::Admin) {
            String username;
            String password;
            tdc::Role role;

            do {
                tdu::clear_term();
                m_printer->msg_print("username: ");
                username = m_input_handler->get_input();
            } while (!m_amanager.username_validation(username));

            do {
                tdu::clear_term();
                m_printer->msg_print("password: ");
                password = m_input_handler->get_secret();
            } while (!m_amanager.password_validation(password));
            tdu::clear_term();

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

            tdc::User user{username, role, password};
            m_user_db->add_object(user);

            m_printer->msg_print("User has been added successfully!");
        }
    });

    user_manager->attach(FIRST_OPTION, edit_users);
    user_manager->attach(SECOND_OPTION, add_new_user);

    const auto advanced = std::make_shared<Page>(
        [] { fmt::print("Advanced:\n[1] Wipe all data\n-> "); });

    const auto wipe_all_data = std::make_shared<Page>(false, [this] {
        m_printer->msg_print("Are you 100% sure?\n-> ");

        if (const auto choice = m_input_handler->get_input(); choice == "yes") {
            tdu::wipe_simple_app_env("2DO");
            tdu::create_simple_app_env("2DO", {DB_NAME, ERR_LOGS_FILE_NAME});
            m_printer->msg_print("Data wiped!");
        } else if (choice == "no") {
        } else {
            invalid_option_event();
        }
    });

    advanced->attach(FIRST_OPTION, wipe_all_data);

    settings->attach(FIRST_OPTION, user_manager);
    settings->attach(SECOND_OPTION, advanced);

    return std::move(settings);
}

void App::invalid_option_event() const {
    m_printer->msg_print("Invalid option!");
    tdu::sleep(2000);
    tdu::clear_term();
};

bool App::user_update_event(UpdateEvent kind, tdc::User& user) {
    switch (kind) {
        case UpdateEvent::UsernameUpdate: {
            String username{};
            do {
                m_printer->msg_print("new username: ");
                username = m_input_handler->get_input();
            } while (!m_amanager.username_validation(username));
            user.set_username(username);
        } break;

        case UpdateEvent::PasswordUpdate: {
            String password{};
            do {
                m_printer->msg_print("new password: ");
                password = m_input_handler->get_secret();
            } while (!m_amanager.password_validation(password));
            user.set_password(password);
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
        } break;
    }
    m_user_db->update_object(user);

    m_printer->msg_print("User updated successfully!");
    tdu::clear_term();
};
}  // namespace twodo