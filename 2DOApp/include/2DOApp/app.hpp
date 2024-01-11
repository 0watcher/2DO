#pragma once

#include <fmt/core.h>
#include <iostream>
#include <memory>

#include <2DOCore/task.hpp>
#include <2DOCore/user.hpp>
#include <Utils/result.hpp>
#include "2DOApp/term.hpp"
#include "Utils/type.hpp"

namespace tdc = twodocore;
namespace tdl = twodoutils;

namespace twodo {
class UserInput : public tdl::IUserInputHandler<String> {
  public:
    String get_input() override {
        auto input = String();
        std::getline(std::cin, input);
        return input;
    }
};

class MsgDisplayer : public tdl::IDisplayer {
  public:
    void msg_display(StringView msg) override { fmt::print("{}", msg); }

    void err_display(StringView err) override { fmt::print("{}", err); }
};

class [[nodiscard]] App {
  public:
    App(App&& other) = default;
    App& operator=(App&& other) = default;
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    App(std::shared_ptr<tdc::User> current_user_,
        std::unique_ptr<tdc::UserDb> user_db_,
        std::unique_ptr<tdc::TaskDb> task_db_)
        : current_user{current_user},
          user_db{std::move(user_db_)},
          task_db{std::move(task_db_)} {}

    void run();
    std::shared_ptr<tdc::User> get_current_user() { return current_user; }

  private:
    std::shared_ptr<tdc::User> current_user;
    std::unique_ptr<tdc::UserDb> user_db;
    std::unique_ptr<tdc::TaskDb> task_db;

    Menu<String> load_menu();
    std::shared_ptr<Page<String>> tasks_menu();
    std::shared_ptr<Page<String>> settings_menu();
};
}  // namespace twodo