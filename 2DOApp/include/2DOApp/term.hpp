#pragma once

#include <functional>
#include <memory>

#include <2DOCore/user.hpp>
#include <Utils/result.hpp>
#include <Utils/type.hpp>
#include <Utils/util.hpp>

namespace tdu = twodoutils;

namespace twodo {
class Page : public std::enable_shared_from_this<Page> {
  public:
    Page(std::function<void()> content)
        : m_content{std::move(content)}, m_menu_event{true} {}

    Page(bool is_menu_event, std::function<void()> content)
        : m_menu_event{is_menu_event}, m_content{std::move(content)} {}

    void execute() const { m_content(); }

    void attach(const String& option, std::shared_ptr<Page> child) {
        m_childs.insert({option, child});
        child->m_parent = this->shared_from_this();
    }

    bool has_any_child() { return !m_childs.empty(); }

  private:
    std::function<void()> m_content;
    std::shared_ptr<Page> m_parent = nullptr;
    HashMap<String, std::shared_ptr<Page>> m_childs{};
    bool m_menu_event;

    std::shared_ptr<Page> get_child(const String& option) const {
        auto it = m_childs.find(option);
        if (it != m_childs.end()) {
            return it->second;
        }
        return nullptr;
    }

    friend class Menu;
};

class [[nodiscard]] Menu {
  public:
    Menu(Menu&&) = default;
    Menu& operator=(Menu&&) = default;
    Menu(const Menu&) = delete;
    Menu& operator=(const Menu&) = delete;

    Menu(std::shared_ptr<Page> initial_page,
         std::shared_ptr<tdu::IPrinter> iprinter_,
         std::shared_ptr<tdu::IUserInputHandler> input_handler_)
        : current_page{std::move(initial_page)},
          printer{iprinter_},
          input_handler{input_handler_} {}

    void run(const String& quit_input) {
        while (true) {
            tdu::clear_term();

            execute_current_page();

            String user_choice = get_user_choice();
            if (handle_quit(user_choice, quit_input)) {
                break;
            }

            navigate_or_display_error(user_choice, quit_input);
        }
    }

  private:
    std::shared_ptr<Page> current_page;
    std::shared_ptr<tdu::IPrinter> printer;
    std::shared_ptr<tdu::IUserInputHandler> input_handler;

    String get_user_choice() const { return input_handler->get_input(); }

    void execute_current_page() {
        if (current_page) {
            current_page->execute();
        }
    }

    bool handle_quit(const String& user_choice, const String& quit_input) {
        if (user_choice == quit_input) {
            return navigate_to_parent_or_exit();
        }
        return false;
    }

    bool navigate_to_parent_or_exit() {
        if (auto parent_page = current_page->m_parent; parent_page) {
            current_page = parent_page;
            return false;
        }
        return true;
    }

    void navigate_or_display_error(const String& user_choice,
                                   const String& quit_input) {
        std::shared_ptr<Page> selected_page =
            current_page->get_child(user_choice);
        if (!selected_page && user_choice != quit_input) {
            display_invalid_option_error();
        } else {
            perform_page_navigation_or_execution(selected_page);
        }
    }

    void display_invalid_option_error() const {
        printer->msg_print("Invalid option!\n");
        tdu::sleep(2000);
    }

    void perform_page_navigation_or_execution(
        std::shared_ptr<Page> selected_page) {
        if (selected_page && !selected_page->m_menu_event) {
            selected_page->execute();
        } else if (selected_page) {
            current_page = selected_page;
        }
    }
};
}  // namespace twodo