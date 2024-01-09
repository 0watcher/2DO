#pragma once

#include <2DOApp/term.hpp>
#include <2DOCore/result.hpp>
#include <2DOCore/user.hpp>
#include <2DOCore/utils.hpp>
#include <memory>

namespace tdc = twodocore;

namespace twodo {
class UserInput : public tdc::IUserInputHandler<String> {
  public:
    String get_input() override {
        auto input = std::string();
        std::getline(std::cin, input);
        return input;
    }
};

class MsgDisplayer : public tdc::IDisplayer {
  public:
    void msg_display(std::string_view msg) override { std::cout << msg; }

    void err_display(std::string_view err) override { std::cerr << err; }
};

class App {
  public:
    App(); 
    void run();
    std::shared_ptr<tdc::User> get_current_user() { return current_user; }

  private:
    std::shared_ptr<tdc::User> current_user = nullptr;
    std::shared_ptr<tdc::UserDb> db = nullptr;
    std::shared_ptr<tdc::TaskDb> db = nullptr;

    Menu<String> load_menu();
    std::shared_ptr<Page<String>> tasks_menu();
    std::shared_ptr<Page<String>> settings_menu();
};
}