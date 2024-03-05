#pragma once

#include <memory>

#include <2DOCore/user.hpp>
#include <Utils/result.hpp>
#include <Utils/type.hpp>
#include <Utils/util.hpp>

namespace tdu = twodoutils;

namespace twodo {
class [[nodiscard]] Page : public std::enable_shared_from_this<Page> {
  public:
    explicit Page(const String& page_name) : m_page_name{page_name} {}

    explicit Page(const std::function<void()>& content)
        : m_content{std::move(content)} {}

    explicit Page(const String& page_name, const std::function<void()>& content)
        : m_content{std::move(content)}, m_page_name{page_name} {}

    explicit Page(bool is_menu_event, const std::function<void()>& content)
        : m_content{std::move(content)}, m_menu_event{is_menu_event} {}

    explicit Page(const String& page_name,
                  const bool is_menu_event,
                  const std::function<void()>& content)
        : m_content{std::move(content)},
          m_page_name{page_name},
          m_menu_event{is_menu_event} {}

    void execute() const { m_content(); }

    void attach(const String& option, std::shared_ptr<Page> child) {
        m_childs.insert({option, child});
        child->m_parent = this->shared_from_this();
    }

  private:
    std::function<void()> m_content{};
    std::shared_ptr<Page> m_parent = nullptr;
    const String m_page_name{};
    HashMap<String, std::shared_ptr<Page>> m_childs{};
    bool m_menu_event = true;

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

    explicit Menu(std::shared_ptr<Page> initial_page,
                  std::shared_ptr<tdu::IPrinter> iprinter_,
                  std::shared_ptr<tdu::IUserInputHandler> input_handler_)
        : m_current_page{std::move(initial_page)},
          m_printer{iprinter_},
          m_input_handler{input_handler_} {}

    void run(const String& quit_input) {
        while (true) {
            tdu::clear_term();

            if (m_current_page->m_content) {
                m_current_page->execute();
            }

            print_menu();

            const String user_choice = m_input_handler->get_input();
            if (handle_quit(user_choice, quit_input)) {
                break;
            }

            navigate_to_page(user_choice, quit_input);
        }
    }

  private:
    std::shared_ptr<Page> m_current_page;
    std::shared_ptr<tdu::IPrinter> m_printer;
    std::shared_ptr<tdu::IUserInputHandler> m_input_handler;

    void print_menu() {
        HashMap<String, String> names;
        for (const auto& page : m_current_page->m_childs) {
            names.insert({page.first, page.second->m_page_name});
        }

        m_printer->menu_print(m_current_page->m_page_name, names);
    }

    bool handle_quit(const String& user_choice, const String& quit_input) {
        if (user_choice == quit_input) {
            return navigate_to_parent_or_exit();
        }
        return false;
    }

    bool navigate_to_parent_or_exit() {
        if (const auto parent_page = m_current_page->m_parent; parent_page) {
            m_current_page = parent_page;
            return false;
        }
        return true;
    }

    void navigate_to_page(const String& user_choice, const String& quit_input) {
        std::shared_ptr<Page> selected_page =
            m_current_page->get_child(user_choice);
        if (!selected_page && user_choice != quit_input) {
            display_invalid_option_error();
        } else {
            perform_page_navigation_or_execution(selected_page);
        }
    }

    void display_invalid_option_error() const {
        m_printer->err_print("Invalid option!");
        tdu::sleep(2000);
    }

    void perform_page_navigation_or_execution(
        std::shared_ptr<Page> selected_page) {
        if (selected_page && !selected_page->m_menu_event) {
            selected_page->execute();
        } else if (selected_page) {
            m_current_page = selected_page;
        }
    }
};
}  // namespace twodo