#pragma once

#include <2DOApp/term.hpp>
#include <2DOCore/result.hpp>
#include <2DOCore/user.hpp>
#include <2DOCore/utils.hpp>
#include <memory>


namespace tdc = twodocore;

namespace twodo
{
class UserInput : public tdc::IUserInputHandler<String>
{
   public:
    String get_input() override
    {
        auto input = std::string();
        std::getline(std::cin, input);
        return input;
    }
};

class MsgDisplayer : public tdc::IDisplayer
{
   public:
    void msg_display(std::string_view msg) override { std::cout << msg; }

    void err_display(std::string_view err) override { std::cerr << err; }
};

class App
{
   public:
    void run();
    std::shared_ptr<tdc::User> get_current_user() { return current_user; }

   private:
    std::shared_ptr<tdc::User> current_user = nullptr;

    Menu<String> load_menu();
    std::shared_ptr<Page<String>> tasks_menu();
    std::shared_ptr<Page<String>> settings_menu();
};
}  // namespace twodo

// namespace twodo
// {
// enum class RunError
// {
//     ExitFailure
// };

// enum class InputError
// {
//     InvalidInput,
// };

// class App
// {
//    public:
//     tdc::Result<tdc::None, RunError> run();

//    private:
//     bool m_is_running = true;

//     tdc::Result<int, InputError> get_valid_option() noexcept;
// };

// class Session
// {
//    public:
//     std::shared_ptr<tdc::User> curr_user;
// };

// }  // namespace twodo