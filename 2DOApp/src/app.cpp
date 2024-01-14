#include "2DOApp/app.hpp"

#define QUIT_KEY "0"

namespace twodo {
void App::run() {
    tdl::create_simple_app_env("2DO", {"users.db3", "tasks.db3"});
    load_menu().run("0");
}

Menu<String> App::load_menu() const {
    std::shared_ptr<Page<String>> main = std::make_shared<Page<String>>([]() {
        fmt::print(
            "Main Menu:\n"
            "[1] Tasks\n"
            "[2] Setting\n"
            "[0] Exit\n"
            "-> ");
    });

    UserInput ui{};
    MsgDisplayer md{};
    Menu menu{main, md, ui};

    main->add_child("1", load_tasks_menu());
    main->add_child("2", load_settings_menu());

    return menu;
}

std::shared_ptr<Page<String>> App::load_tasks_menu() const {
    std::shared_ptr<Page<String>> tasks = std::make_shared<Page<String>>([]() {
        fmt::print(
            "Tasks:\n"
            "[1] Your tasks\n"
            "[2] Delegated tasks\n"
            "[3] Create task\n"
            "[0] Back\n"
            "-> ");
    });

    std::shared_ptr<Page<String>> your_tasks =
        std::make_shared<Page<String>>([]() { fmt::print("Your tasks:\n"); });

    std::shared_ptr<Page<String>> delegated_tasks =
        std::make_shared<Page<String>>([]() {
            fmt::print(
                "Tasks:\n"
                "[1] Your tasks\n"
                "[2] Delegated tasks\n"
                "[3] Create task\n"
                "[0] Back\n"
                "-> ");
        });

    std::shared_ptr<Page<String>> create_task =
        std::make_shared<Page<String>>([]() {

        });

    return tasks;
}

std::shared_ptr<Page<String>> App::load_settings_menu() const {
    std::shared_ptr<Page<String>> settings =
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