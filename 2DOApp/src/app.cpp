#include "2DOApp/app.hpp"
#include <memory>
#include <stdexcept>
#include "fmt/core.h"

namespace twodo {
void App::run() {
    // tdu::create_simple_app_env("2DO", {DB_NAME, ERR_LOGS_FILE_NAME,
    // USER_LOGS_FILE_NAME});
    load_menu().run(QUIT_OPTION);
}

Menu<String> App::load_menu() const {
    auto main = load_main_menu();
    Menu<String> menu{main, std::make_unique<MsgDisplayer>(),
                      std::make_unique<UserInput>()};

    main->attach(FIRST_OPTION, load_tasks_menu());
    main->attach(SECOND_OPTION, load_settings_menu());

    return menu;
}

std::shared_ptr<Page<String>> App::load_main_menu() const {
    const std::shared_ptr<Page<String>> main =
        std::make_shared<Page<String>>([]() {
            fmt::print(
                "Main Menu:\n"
                "[1] Tasks\n"
                "[2] Setting\n"
                "[0] Exit\n"
                "-> ");
        });

    return main;
}

std::shared_ptr<Page<String>> App::load_tasks_menu() const {
    const std::shared_ptr<Page<String>> tasks =
        std::make_shared<Page<String>>([]() {
            fmt::print(
                "Tasks:\n"
                "[1] Your tasks\n"
                "[2] Delegated tasks\n"
                "[3] Create task\n"
                "[0] Back\n"
                "-> ");
        });

    const std::shared_ptr<Page<String>> your_tasks =
        std::make_shared<Page<String>>([this]() {
            fmt::print("Your tasks:\n");
            auto tasks = task_db.get_all_objects(current_user->id());
            if (!tasks) {
                throw std::runtime_error("Failure selecting tasks!");
            }

            unsigned int count = 1;
            for (const auto& task : tasks.unwrap()) {
                const auto owner = user_db.get_object(task.owner_id()).unwrap();
                fmt::println("[{}] {} from {} ({})", count++, task.topic(),
                             owner.username(), str_done(task.is_done()));
            }
        });

    const std::shared_ptr<Page<String>> delegated_tasks =
        std::make_shared<Page<String>>([]() { fmt::print(""); });

    const std::shared_ptr<Page<String>> create_task =
        std::make_shared<Page<String>>([]() {

        }, false);

    tasks->attach(FIRST_OPTION, your_tasks);
    tasks->attach(SECOND_OPTION, delegated_tasks);
    tasks->attach(THIRD_OPTION, create_task);

    return tasks;
}

std::shared_ptr<Page<String>> App::load_settings_menu() const {
    const std::shared_ptr<Page<String>> settings =
        std::make_shared<Page<String>>([]() {
            fmt::print(
                "Settings:\n"
                "[1] Manage users\n"
                "[2] Advanced\n"
                "[0] Back\n"
                "-> ");
        });

    const std::shared_ptr<Page<String>> manage_users =
        std::make_shared<Page<String>>([this]() {
            auto users = user_db.get_all_objects().unwrap();

            unsigned int count = 1;
            for (const auto& user : users) {
                fmt::println("[{}] {} ({})", count++, user.username(), user.role<String>());
            }
        });

    const std::shared_ptr<Page<String>> advanced =
        std::make_shared<Page<String>>([]() {
            fmt::println("Advanced:\n[1] Wipe all data");
        });

    const std::shared_ptr<Page<String>> wipe_all_data =
        std::make_shared<Page<String>>([]() {
            fmt::println("Are you 100% sure?");
        }, false);

    advanced->attach(FIRST_OPTION, wipe_all_data);
    
    settings->attach(FIRST_OPTION, manage_users);
    settings->attach(SECOND_OPTION, advanced);

    return settings;
}

StringView App::str_done(bool is_done) const {
    if (is_done)
        return "DONE";
    else
        return "INCOMPLETE";
};
}  // namespace twodo