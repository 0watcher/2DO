#include <fmt/core.h>
#include <iostream>
#include <memory>

#include <2DOApp/app.hpp>
#include <Utils/type.hpp>
#include <Utils/util.hpp>

namespace td = twodo;
namespace tdu = twodoutils;

class UserInput : public tdu::IUserInputHandler {
  public:
    String get_input() const override {
        auto input = String();
        std::getline(std::cin, input);
        return input;
    }
};

class MsgDisplayer : public tdu::IPrinter {
  public:
    void msg_print(StringView msg) const override { fmt::print("{}", msg); }

    void err_print(StringView err) const override {
        fmt::print(stderr, "{}", err);
    }
};

int main() {
    try {
        td::App::getInstance()
            ->set_dependencies(std::make_shared<MsgDisplayer>(),
                               std::make_shared<UserInput>())
            ->run();
    } catch (const std::runtime_error& e) {
        tdu::log_to_file(e.what(), ERR_LOGS_FILE_NAME);
        fmt::print(stderr, "{}", e.what());
    }
}