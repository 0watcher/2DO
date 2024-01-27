#pragma once

#include <fmt/core.h>
#include <iostream>
#include <memory>

#include <2DOApp/term.hpp>
#include <2DOCore/task.hpp>
#include <2DOCore/user.hpp>
#include <Utils/result.hpp>
#include <Utils/type.hpp>

namespace tdc = twodocore;
namespace tdu = twodoutils;

#define QUIT_OPTION "0"
#define FIRST_OPTION "1"
#define SECOND_OPTION "2"
#define THIRD_OPTION "2"
#define DB_NAME "2do.db3"
#define ERR_LOGS_FILE_NAME "big-error-logs.txt"
#define USER_LOGS_FILE_NAME "user-logs.txt"

namespace twodo {
class UserInput : public tdu::IUserInputHandler<String> {
  public:
    String get_input() const override {
        auto input = String();
        std::getline(std::cin, input);
        return input;
    }
};

class MsgDisplayer : public tdu::IPrinter {
  public:
    void msg_print(StringView msg) const override { fmt::println("{}", msg); }

    void err_print(StringView err) const override {
        fmt::println(stderr, "{}", err);
    }
};

class [[nodiscard]] App {
  public:
    App(App&& other) = default;
    App& operator=(App&& other) = default;
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    static std::shared_ptr<App> getInstance() {
        if (instance.get() == nullptr) {
            instance = std::shared_ptr<App>();
        }

        return instance;
    }

    void run();
    std::shared_ptr<tdc::User> get_current_user() { return current_user; }

  private:
    inline static std::shared_ptr<App> instance = nullptr;
    std::shared_ptr<tdc::User> current_user = nullptr;
    tdc::UserDb user_db{DB_NAME};
    tdc::TaskDb task_db{DB_NAME};
    tdc::MessageDb message_db{DB_NAME};

    App() = default;
    Menu<String> load_menu() const;
    std::shared_ptr<Page<String>> load_main_menu() const;
    std::shared_ptr<Page<String>> load_tasks_menu() const;
    std::shared_ptr<Page<String>> load_settings_menu() const;

    StringView str_done(bool is_done) const;
};
}  // namespace twodo