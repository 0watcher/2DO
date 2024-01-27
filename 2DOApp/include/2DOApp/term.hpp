#pragma once

#include <fmt/core.h>
#include <functional>
#include <memory>

#include <2DOCore/user.hpp>
#include <Utils/result.hpp>
#include <Utils/type.hpp>
#include <Utils/util.hpp>

namespace tdu = twodoutils;
namespace tdc = twodocore;

namespace twodo {
template <typename TOption>
class [[nodiscard]] Page : public std::enable_shared_from_this<Page<TOption>> {
  public:
    Page(const Page&) = default;
    Page& operator=(const Page&) = default;
    Page(Page&& other) = default;
    Page& operator=(Page&& other) = default;

    Page(std::function<void()> content,
         std::shared_ptr<Page> parent = nullptr,
         bool is_page_event = true)
        : content{std::move(content)},
          parent{parent},
          page_event{is_page_event} {}

    void execute() const { content(); }

    void attach(TOption option, std::shared_ptr<Page<TOption>> child) {
        childs.insert({option, child});
        child->parent = this->shared_from_this();
    }

    std::shared_ptr<Page> get_parent() const { return parent; }

    std::shared_ptr<Page<TOption>> get_page(TOption option) const {
        auto it = childs.find(option);
        if (it != childs.end()) {
            return it->second;
        }
        return nullptr;
    }

  private:
    std::function<void()> content;
    std::shared_ptr<Page<TOption>> parent;
    HashMap<TOption, std::shared_ptr<Page<TOption>>> childs;

  public:
    bool page_event;
};

template <typename TOption>
class [[nodiscard]] Menu {
  public:
    Menu(Menu&& other) = default;
    Menu& operator=(Menu&& other) = default;
    Menu(const Menu&) = delete;
    Menu& operator=(const Menu&) = delete;

    Menu(std::shared_ptr<Page<TOption>> initial_page,
         std::unique_ptr<tdu::IPrinter> iprinter,
         std::unique_ptr<tdu::IUserInputHandler<TOption>> input_handler_)
        : current_page{std::move(initial_page)},
          printer{std::move(iprinter)},
          input_handler{std::move(input_handler_)} {}

    void run(TOption quit_input = TOption{}) {
        while (true) {
            execute_current_page();

            TOption user_choice = get_user_choice();
            if (handle_quit(user_choice, quit_input)) {
                break;
            }

            navigate_or_display_error(user_choice, quit_input);

            tdu::clear_term();
        }
    }

  private:
    std::shared_ptr<Page<TOption>> current_page;
    std::unique_ptr<tdu::IPrinter> printer;
    std::unique_ptr<tdu::IUserInputHandler<TOption>> input_handler;

    TOption get_user_choice() const { return input_handler->get_input(); }

    void execute_current_page() {
        if (current_page) {
            current_page->execute();
        }
    }

    bool handle_quit(TOption user_choice, TOption quit_input) {
        if (user_choice == quit_input) {
            return navigate_to_parent_or_exit();
        }
        return false;
    }

    bool navigate_to_parent_or_exit() {
        const auto parent_page = current_page->get_parent();
        if (!parent_page) {
            return true;
        }
        current_page = parent_page;
        return false;
    }

    void navigate_or_display_error(TOption user_choice, TOption quit_input) {
        std::shared_ptr<Page<TOption>> selected_page =
            current_page->get_page(user_choice);
        if (!selected_page && user_choice != quit_input) {
            display_invalid_option_error();
        } else {
            perform_page_navigation_or_execution(selected_page);
        }
    }

    void display_invalid_option_error() const {
        printer->msg_print("Invalid option!");
        tdu::sleep(2000);
    }

    void perform_page_navigation_or_execution(
        std::shared_ptr<Page<TOption>> selected_page) {
        if (selected_page && !selected_page->page_event) {
            selected_page->execute();
        } else if (selected_page) {
            current_page = selected_page;
        }
    }
};

enum class AuthErr {
    InvalidNameLength = 1,
    AlreadyExistingName,
    InvalidPassLength,
    MissingUpperCase,
    MissingLowerCase,
    MissingNumber,
    MissingSpecialCharacter,
    UserNotFound,
    AllTriesExhausted,
    DbErr,
};

class [[nodiscard]] RegisterManager {
  public:
    RegisterManager(RegisterManager&& other) = default;
    RegisterManager& operator=(RegisterManager&& other) = default;
    RegisterManager(const RegisterManager&) = delete;
    RegisterManager& operator=(const RegisterManager&) = delete;

    RegisterManager(std::shared_ptr<tdc::UserDb> udb,
                    std::shared_ptr<tdu::IUserInputHandler<String>> ihandler,
                    std::shared_ptr<tdu::IPrinter> iprinter)
        : m_udb{udb}, m_ihandler{ihandler}, m_printer{iprinter} {}

    [[nodiscard]] tdu::Result<tdc::User, AuthErr> singup();

    [[nodiscard]] tdu::Result<void, AuthErr> username_validation(
        StringView username) const;

    [[nodiscard]] tdu::Result<void, AuthErr> password_validation(
        const String& password) const;

  private:
    std::shared_ptr<tdc::UserDb> m_udb;
    std::shared_ptr<tdu::IUserInputHandler<String>> m_ihandler;
    std::shared_ptr<tdu::IPrinter> m_printer;
};

class [[nodiscard]] AuthManager {
  public:
    AuthManager(AuthManager&& other) = default;
    AuthManager& operator=(AuthManager&& other) = default;
    AuthManager(const AuthManager&) = delete;
    AuthManager& operator=(const AuthManager&) = delete;

    AuthManager(std::shared_ptr<tdc::UserDb> udb,
                std::shared_ptr<tdu::IUserInputHandler<String>> ihandler,
                std::shared_ptr<tdu::IPrinter> iprinter)
        : m_udb{udb}, m_ihandler{ihandler}, m_printer{iprinter} {}

    [[nodiscard]] tdu::Result<tdc::User, AuthErr> login();

    [[nodiscard]] tdu::Result<void, AuthErr> auth_username();

    [[nodiscard]] tdu::Result<tdc::User, AuthErr> auth_password(
        const String& username);

  private:
    std::shared_ptr<tdc::UserDb> m_udb;
    std::shared_ptr<tdu::IUserInputHandler<String>> m_ihandler;
    std::shared_ptr<tdu::IPrinter> m_printer;
};
}  // namespace twodo