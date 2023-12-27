#include <2DOCore/result.hpp>
#include <2DOCore/user.hpp>
#include <2DOCore/utils.hpp>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace tdc = twodocore;

namespace twodo
{
template <typename TOption>
class Page : public std::enable_shared_from_this<Page<TOption>>
{
   public:
    Page(std::function<void()> content, std::shared_ptr<Page> parent = nullptr,
         bool is_input_event = false)
        : content {std::move(content)}, parent {parent}, input_event {is_input_event}
    {
    }

    void execute() const { content(); }

    void add_child(TOption option, std::shared_ptr<Page<TOption>> child)
    {
        childs.insert({option, child});
        child->parent = this->shared_from_this();
    }

    std::shared_ptr<Page> getParent() const { return parent; }

    std::shared_ptr<Page<TOption>> get_page(TOption option) const
    {
        auto it = childs.find(option);
        if (it != childs.end())
        {
            return it->second;
        }
        else
        {
            return nullptr;
        }
    }

    bool input_event;

   private:
    std::function<void()> content;
    std::shared_ptr<Page<TOption>> parent;
    std::unordered_map<TOption, std::shared_ptr<Page<TOption>>> childs;
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

    void run(TOption quit_input = TOption{})
    {
        while (true)
        {
            if (current_page)
            {
                current_page->execute();
            }

            TOption choice = getOption();
            if(choice == quit_input) break;

            std::shared_ptr<Page<TOption>> page = nullptr;
            if (page = current_page->get_page(choice); !page)
            {
                displayer.msg_display("Invalid option!");
                tdc::sleep(2000);
            }

            if (page->input_event)
            {
                page->execute();
            }
            else
            {
                current_page = page;
            }

            tdc::clear_term();
        }
    }

   private:
    std::shared_ptr<Page<TOption>> current_page;
    tdc::IDisplayer& displayer;
    tdc::IUserInputHandler<TOption>& input_handler;

    TOption get_option()
    {
        displayer.msg_display("->");
        return input_handler.get_input();
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