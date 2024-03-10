#pragma once

#include <memory>

#include <2DOCore/user.hpp>
#include <Utils/result.hpp>
#include <Utils/type.hpp>
#include <Utils/util.hpp>

namespace tdu = twodoutils;

namespace twodocore {
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

    void execute() const;

    void attach(const String& option, std::shared_ptr<Page> child);

  private:
    std::function<void()> m_content{};
    std::shared_ptr<Page> m_parent = nullptr;
    const String m_page_name{};
    HashMap<String, std::shared_ptr<Page>> m_childs{};
    bool m_menu_event = true;

    std::shared_ptr<Page> get_child(const String& option) const;

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

    void run(const String& quit_input);

    void back_to(const String& page_name);

  private:
    std::shared_ptr<Page> m_current_page;
    std::shared_ptr<tdu::IPrinter> m_printer;
    std::shared_ptr<tdu::IUserInputHandler> m_input_handler;

    void print_menu() const;

    bool handle_quit(const String& user_choice, const String& quit_input);

    bool navigate_to_parent_or_exit();

    void navigate_to_page(const String& user_choice, const String& quit_input);

    void display_invalid_option_error() const;

    void perform_page_navigation_or_execution(
        std::shared_ptr<Page> selected_page);
};
}  // namespace twodocore