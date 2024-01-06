#include <2DOCore/result.hpp>
#include <2DOCore/user.hpp>
#include <2DOCore/utils.hpp>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "fmt/core.h"

namespace tdc = twodocore;

namespace twodo
{
template <typename TOption>
class Page : public std::enable_shared_from_this<Page<TOption>>
{
   public:
    Page(std::function<void()> content, std::shared_ptr<Page> parent = nullptr,
         bool is_page_event = true)
        : content {std::move(content)}, parent {parent}, page_event {is_page_event}
    {
    }

    void execute() const { content(); }

    void add_child(TOption option, std::shared_ptr<Page<TOption>> child)
    {
        childs.insert({option, child});
        child->parent = this->shared_from_this();
    }

    std::shared_ptr<Page> get_parent() const { return parent; }

    std::shared_ptr<Page<TOption>> get_page(TOption option) const
    {
        auto it = childs.find(option);
        if (it != childs.end())
        {
            return it->second;
        }
        return nullptr;
    }

   private:
    std::function<void()> content;
    std::shared_ptr<Page<TOption>> parent;
    std::unordered_map<TOption, std::shared_ptr<Page<TOption>>> childs;

   public:
    bool page_event;
};

template <typename TOption>
class Menu
{
   public:
    Menu(std::shared_ptr<Page<TOption>> initial_page, tdc::IDisplayer& displayer,
         tdc::IUserInputHandler<TOption>& input_handler_)
        : current_page {std::move(initial_page)},
          displayer {displayer},
          input_handler {input_handler_}
    {
    }

    void run(TOption quit_input = TOption {})
    {
        while (true)
        {
            execute_current_page();

            TOption user_choice = get_user_choice();
            if (handle_quit(user_choice, quit_input))
            {
                break;
            }

            navigate_or_display_error(user_choice, quit_input);

            tdc::clear_term();
        }
    }

   private:
    std::shared_ptr<Page<TOption>> current_page;
    tdc::IDisplayer& displayer;
    tdc::IUserInputHandler<TOption>& input_handler;

    TOption get_user_choice() { return input_handler.get_input(); }

    void execute_current_page()
    {
        if (current_page)
        {
            current_page->execute();
        }
    }

    bool handle_quit(TOption user_choice, TOption quit_input)
    {
        if (user_choice == quit_input)
        {
            return navigate_to_parent_or_exit();
        }
        return false;
    }

    bool navigate_to_parent_or_exit()
    {
        const auto parent_page = current_page->get_parent();
        if (!parent_page)
        {
            return true;
        }
        current_page = parent_page;
        return false;
    }

    void navigate_or_display_error(TOption user_choice, TOption quit_input)
    {
        std::shared_ptr<Page<TOption>> selected_page = current_page->get_page(user_choice);
        if (!selected_page && user_choice != quit_input)
        {
            display_invalid_option_error();
        }
        else
        {
            perform_page_navigation_or_execution(selected_page);
        }
    }

    void display_invalid_option_error()
    {
        displayer.msg_display("Invalid option!");
        tdc::sleep(2000);
    }

    void perform_page_navigation_or_execution(std::shared_ptr<Page<TOption>> selected_page)
    {
        if (selected_page && !selected_page->page_event)
        {
            selected_page->execute();
        }
        else if (selected_page)
        {
            current_page = selected_page;
        }
    }
};

enum class AuthErr
{
    InvalidNameLength = 1,
    AlreadyExistingName,
    PasswordTooShort,
    MissingUpperCase,
    MissingLowerCase,
    MissingNumber,
    MissingSpecialCharacter,
    UserNotFound,
    AllTriesExhausted,
    DbErr,
};

class RegisterManager
{
   public:
    RegisterManager(std::shared_ptr<tdc::UserDb> udb,
                    std::shared_ptr<tdc::IUserInputHandler<String>> ihandler,
                    std::shared_ptr<tdc::IDisplayer> idisplayer)
        : m_udb {udb}, m_ihandler {ihandler}, m_idisplayer {idisplayer}
    {
    }

    [[nodiscard]] tdc::Result<tdc::User, AuthErr> singup();
    [[nodiscard]] tdc::Result<tdc::None, AuthErr> username_validation(
        std::string_view username) const;
    [[nodiscard]] tdc::Result<tdc::None, AuthErr> password_validation(const String& password) const;

   private:
    std::shared_ptr<tdc::UserDb> m_udb;
    std::shared_ptr<tdc::IUserInputHandler<String>> m_ihandler;
    std::shared_ptr<tdc::IDisplayer> m_idisplayer;
};

class AuthManager
{
   public:
    AuthManager(std::shared_ptr<tdc::UserDb> udb,
                std::shared_ptr<tdc::IUserInputHandler<String>> ihandler,
                std::shared_ptr<tdc::IDisplayer> idisplayer)
        : m_udb {udb}, m_ihandler {ihandler}, m_idisplayer {idisplayer}
    {
    }

    [[nodiscard]] tdc::Result<tdc::User, AuthErr> login();
    [[nodiscard]] tdc::Result<tdc::None, AuthErr> auth_username();
    [[nodiscard]] tdc::Result<tdc::User, AuthErr> auth_password(const String& username);

   private:
    std::shared_ptr<tdc::UserDb> m_udb;
    std::shared_ptr<tdc::IUserInputHandler<String>> m_ihandler;
    std::shared_ptr<tdc::IDisplayer> m_idisplayer;
};
}  // namespace twodo