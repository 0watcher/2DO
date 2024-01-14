#include "2DOApp/app.hpp"
#include <memory>
#include "fmt/core.h"

#define QUIT_OPTION "0"
#define FIRST_OPTION "1"
#define SECOND_OPTION "2"

namespace twodo {
void App::run() {
    //tdl::create_simple_app_env("2DO", {"users.db3", "tasks.db3"});
    load_menu().run(QUIT_OPTION);
}

Menu<String> App::load_menu() const {
    const std::shared_ptr<Page<String>> main = std::make_shared<Page<String>>([]() {
        fmt::print(
            "Main Menu:\n"
            "[1] Tasks\n"
            "[2] Setting\n"
            "[0] Exit\n"
            "-> ");
    });

    Menu<String> menu{main, std::make_unique<MsgDisplayer>(), std::make_unique<UserInput>()};

    main->add_child(FIRST_OPTION, load_tasks_menu());
    main->add_child(SECOND_OPTION, load_settings_menu());

    return menu;
}

std::shared_ptr<Page<String>> App::load_tasks_menu() const {
    const std::shared_ptr<Page<String>> tasks = std::make_shared<Page<String>>([]() {
        fmt::print(
            "Tasks:\n"
            "[1] Your tasks\n"
            "[2] Delegated tasks\n"
            "[3] Create task\n"
            "[0] Back\n"
            "-> ");
    });

    const std::shared_ptr<Page<String>> your_tasks =
        std::make_shared<Page<String>>([]() { fmt::print("Your tasks:\n"); });

    const std::shared_ptr<Page<String>> delegated_tasks =
        std::make_shared<Page<String>>([]() {
            fmt::print(
                "Tasks:\n"
                "[1] Your tasks\n"
                "[2] Delegated tasks\n"
                "[3] Create task\n"
                "[0] Back\n"
                "-> ");
        });

    const std::shared_ptr<Page<String>> create_task =
        std::make_shared<Page<String>>([]() {

        });

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

    return settings;
}
}  // namespace twodo