#include "2DOApp/app.hpp"

#include <fmt/core.h>

#include <2DOApp/term.hpp>
#include <2DOCore/user.hpp>
#include <Utils/type.hpp>
#include <Utils/util.hpp>

namespace twodo {
void App::run() {
    // tdu::create_simple_app_env("2DO", {DB_NAME, ERR_LOGS_FILE_NAME,
    // USER_LOGS_FILE_NAME});
    load_menu().run(QUIT_OPTION);
}

Menu App::load_menu() {
    auto main = load_main_menu();
    Menu menu{main, m_printer, m_input_handler};

    main->attach(FIRST_OPTION, load_tasks_menu());
    main->attach(SECOND_OPTION, load_settings_menu());

    return std::move(menu);
}

std::shared_ptr<Page> App::load_main_menu() const {
    const auto main = std::make_shared<Page>([this]() {
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
    const std::shared_ptr<Page> tasks = std::make_shared<Page>([this]() {
        m_printer->msg_print(
            "Tasks:\n"
            "[1] Your tasks\n"
            "[2] Delegated tasks\n"
            "[3] Create task\n"
            "[0] Back\n"
            "-> ");
    });

    const std::shared_ptr<Page> your_tasks = std::make_shared<Page>([this]() {
        m_printer->msg_print("Your tasks:\n");
        auto tasks = m_task_db->get_all_objects(m_current_user->id());

        unsigned int count = 1;
        for (const auto& task : tasks) {
            const auto owner = m_user_db->get_object(task.owner_id());
            m_printer->msg_print(fmt::format("[{}] {} from {} ({})", count++,
                                             task.topic(), owner.username(),
                                             str_done(task.is_done())));
        }
    });

    const std::shared_ptr<Page> delegated_tasks =
        std::make_shared<Page>([]() { fmt::print(""); });

    const std::shared_ptr<Page> create_task = std::make_shared<Page>(
        []() {

        },
        false);

    tasks->attach(FIRST_OPTION, your_tasks);
    tasks->attach(SECOND_OPTION, delegated_tasks);
    tasks->attach(THIRD_OPTION, create_task);

    return std::move(tasks);
}

std::shared_ptr<Page> App::load_settings_menu() {
    const std::shared_ptr<Page> settings = std::make_shared<Page>([this]() {
        m_printer->msg_print(
            "Settings:\n"
            "[1] User Manager\n"
            "[2] Advanced\n"
            "[0] Back\n"
            "-> ");
    });

    const std::shared_ptr<Page> user_manager = std::make_shared<Page>([this]() {
        m_printer->msg_print(
            "User Manager:\n"
            "[1] Manage users\n"
            "[2] Create new user\n"
            "[0] Back\n"
            "-> ");
    });

    const std::shared_ptr<Page> update_users =
        std::make_shared<Page>([this]() {
            auto users = m_user_db->get_all_objects();

            unsigned int count = 1;
            for (const auto& user : users) {
                m_printer->msg_print(fmt::format("Users:\n[{}] {} ({})\n-> ",
                                                 count++, user.username(),
                                                 user.role<String>()));
            }
        });

    const std::shared_ptr<Page> add_new_user = std::make_shared<Page>(
        [this]() {
            if (m_current_user->role<tdc::Role>() == tdc::Role::Admin) {
                String username;
                String password;
                tdc::Role role;

                while (!m_amanager.username_validation(username)) {
                    tdu::clear_term();
                    username = m_input_handler->get_input();
                }
                tdu::clear_term();

                while (!m_amanager.password_validation(password)) {
                    password = m_input_handler->get_input();
                }
                tdu::clear_term();

                while (true) {
                    m_printer->msg_print("Role:\n[1] Admin\n[2] User\n-> ");
                    String str_role = m_input_handler->get_input();
                    if (str_role == FIRST_OPTION) {
                        role = tdc::Role::Admin;
                    } else if (str_role == SECOND_OPTION) {
                        role = tdc::Role::User;
                    } else {
                        m_printer->msg_print("Invalid option!");
                        tdu::sleep(2000);
                        tdu::clear_term();
                    }
                }

                tdc::User user{username, role, password};
                m_user_db->add_object(user);

                m_printer->msg_print("User has been added successfully!");
            }
        },
        false);

    user_manager->attach(FIRST_OPTION, update_users);
    user_manager->attach(SECOND_OPTION, add_new_user);

    const std::shared_ptr<Page> users = std::make_shared<Page>(
        []() { fmt::print("Advanced:\n[1] Wipe all data\n-> "); });

    const std::shared_ptr<Page> advanced = std::make_shared<Page>(
        []() { fmt::print("Advanced:\n[1] Wipe all data\n-> "); });

    const std::shared_ptr<Page> wipe_all_data = std::make_shared<Page>(
        [this]() {
            m_printer->msg_print("Are you 100% sure?\n-> ");

            if (const auto choice = m_input_handler->get_input();
                choice == "yes") {
                tdu::wipe_simple_app_env("2DO");
                tdu::create_simple_app_env("2DO",
                                           {DB_NAME, ERR_LOGS_FILE_NAME});
                m_printer->msg_print("Data wiped!");
            } else if (choice == "no") {
            } else {
                m_printer->msg_print("Invalid option!");
                tdu::sleep(2000);
                tdu::clear_term();
            }
        },
        false);

    advanced->attach(FIRST_OPTION, wipe_all_data);

    settings->attach(FIRST_OPTION, user_manager);
    settings->attach(SECOND_OPTION, advanced);

    return std::move(settings);
}

StringView App::str_done(bool is_done) const {
    if (is_done)
        return "DONE";
    else
        return "INCOMPLETE";
};
}  // namespace twodo