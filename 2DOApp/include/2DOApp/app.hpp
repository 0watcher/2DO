#pragma once

#include <fmt/core.h>
#include <cerrno>
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
    String get_input() const override {
        auto input = String();
        std::getline(std::cin, input);
        return input;
    }
};

class MsgDisplayer : public tdl::IDisplayer {
  public:
    void msg_display(StringView msg) const override { fmt::print("{}", msg); }

    void err_display(StringView err) const override { fmt::print(stderr, "{}", err); }
};

class [[nodiscard]] App {
  public:
    App(App&& other) = default;
    App& operator=(App&& other) = default;
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    static std::shared_ptr<App> getInstance() {
        if(instance.get() == nullptr) {
            instance = std::make_shared<App>();
        }

        return instance;
    }

    void run();
    std::shared_ptr<tdc::User> get_current_user() { return current_user; }

  private:
    inline static std::shared_ptr<App> instance = nullptr;
    std::shared_ptr<tdc::User> current_user;
    std::unique_ptr<tdc::UserDb> user_db;
    std::unique_ptr<tdc::TaskDb> task_db;

    Menu<String> load_menu() const;
    std::shared_ptr<Page<String>> load_tasks_menu() const;
    std::shared_ptr<Page<String>> load_settings_menu() const;
};
}  // namespace twodo