#include <conio.h>
#include <filesystem>
#include <iostream>
#include <memory>

#include <fmt/color.h>
#include <fmt/core.h>

#include <2DOApp/app.hpp>
#include <Utils/type.hpp>
#include <Utils/util.hpp>
#include <string>

namespace td = twodo;
namespace tdu = twodoutils;

class UserInput : public tdu::IUserInputHandler {
  public:
    String get_input() const override {
        auto input = String();

        std::cin.clear();
        std::getline(std::cin, input);

        return input;
    }

    String get_secret() const override {
        String secret;

        char ch;

#ifdef _WIN32
        while ((ch = _getch()) != '\r') {
#else
        while ((ch = _getch()) != '\n') {

#endif
            if (ch == '\b') {
                if (!secret.empty()) {
                    fmt::print("\b \b");

                    secret.pop_back();
                }
            } else {
                std::cout << '*';
                secret += ch;
            }
        }

        return secret;
    }
};

class MsgDisplayer : public tdu::IPrinter {
  public:
    void msg_print(StringView msg) const override { fmt::print("{}", msg); }

    void err_print(StringView msg) const override {
        fmt::print("{}", fmt::format(fmt::fg(fmt::color::red), msg));
    }

    void menu_print(StringView page_name,
                    const HashMap<String, String>& menu_pages) const override {
        if (!page_name.empty()) {
            fmt::println("{}",
                         fmt::format(fg(fmt::color::lime_green), page_name));
        }

        bool is_some_empty = true;
        for (const auto& page : menu_pages) {
            if (!page.first.empty() && !page.second.empty()) {
                fmt::println(
                    "[{}] {}",
                    fmt::format(fg(fmt::color::blue_violet), page.first),
                    fmt::format(fg(fmt::color::blue_violet), page.second));
                is_some_empty = false;
            }
        }

        if (is_some_empty) {
            fmt::print("\n");
        }
        fmt::println("[{}] {}", fmt::format(fg(fmt::color::blue_violet), "0"),
                     fmt::format(fg(fmt::color::blue_violet), "Back"));
        fmt::print("-> ");
    }
};

int main() {
    try {
        td::App::getInstance()
            ->set_dependencies(std::make_shared<MsgDisplayer>(),
                               std::make_shared<UserInput>())
            ->run();
    } catch (const std::runtime_error& e) {
        tdu::log_to_file(e.what(), fs::current_path().root_path() / ENV_FOLDER_NAME / ERR_LOGS_FILE_NAME);
        fmt::print(stderr, "Error: {}", std::move(e.what()));
    }
}