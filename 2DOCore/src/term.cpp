#include "2DOCore/term.hpp"

namespace twodocore {
void Page::execute() const {
    m_content();
}

void Page::attach(const String& option, std::shared_ptr<Page> child) {
    m_childs.insert({option, child});
    child->m_parent = this->shared_from_this();
}

std::shared_ptr<Page> Page::get_child(const String& option) const {
    auto it = m_childs.find(option);
    if (it != m_childs.end()) {
        return it->second;
    }
    return nullptr;
}

void Menu::run(const String& quit_input) {
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

void Menu::back_to(const String& page_name) {
    std::shared_ptr<Page> currentPage = m_current_page;

    while (currentPage) {
        if (currentPage->m_page_name == page_name) {
            m_current_page = currentPage;
            break;
        }

        currentPage = currentPage->m_parent;
    }
}

void Menu::print_menu() const {
    HashMap<String, String> names;
    for (const auto& page : m_current_page->m_childs) {
        names.insert({page.first, page.second->m_page_name});
    }

    m_printer->menu_print(m_current_page->m_page_name, names);
}

bool Menu::handle_quit(const String& user_choice, const String& quit_input) {
    if (user_choice == quit_input) {
        return navigate_to_parent_or_exit();
    }
    return false;
}

bool Menu::navigate_to_parent_or_exit() {
    if (const auto parent_page = m_current_page->m_parent; parent_page) {
        m_current_page = parent_page;
        return false;
    }
    return true;
}

void Menu::navigate_to_page(const String& user_choice,
                            const String& quit_input) {
    std::shared_ptr<Page> selected_page =
        m_current_page->get_child(user_choice);
    if (!selected_page && user_choice != quit_input) {
        display_invalid_option_error();
    } else {
        perform_page_navigation_or_execution(selected_page);
    }
}

void Menu::display_invalid_option_error() const {
    m_printer->err_print("Invalid option!");
    tdu::sleep(2000);
}

void Menu::perform_page_navigation_or_execution(
    std::shared_ptr<Page> selected_page) {
    if (selected_page && !selected_page->m_menu_event) {
        selected_page->execute();
    } else if (selected_page) {
        m_current_page = selected_page;
    }
}
}  // namespace twodocore