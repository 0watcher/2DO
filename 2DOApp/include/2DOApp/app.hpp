#pragma once

#include <memory>

#include <2DOApp/term.hpp>
#include <2DOCore/task.hpp>
#include <2DOCore/user.hpp>
#include <Utils/result.hpp>
#include <Utils/type.hpp>
#include "Utils/util.hpp"

namespace tdc = twodocore;
namespace tdu = twodoutils;

#define QUIT_OPTION "0"
#define FIRST_OPTION "1"
#define SECOND_OPTION "2"
#define THIRD_OPTION "3"
#define DB_NAME ":memory:"
#define ERR_LOGS_FILE_NAME "big-error-logs.txt"
#define USER_LOGS_FILE_NAME "user-logs.txt"

namespace twodo {
class [[nodiscard]] App {
  public:
    App(App&& other) = default;
    App& operator=(App&& other) = default;
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    App() = default;

    static std::shared_ptr<App> getInstance() {
        if (!instance) {
            instance = std::make_shared<App>();
        }
        return instance;
    }

    std::shared_ptr<App> set_dependencies(
        std::shared_ptr<tdu::IPrinter> iprinter,
        std::shared_ptr<tdu::IUserInputHandler> inputhandler) {
        instance->m_printer = iprinter;
        instance->m_input_handler = inputhandler;
        return instance;
    }

    void run();

  private:
    inline static std::shared_ptr<App> instance = nullptr;
    std::shared_ptr<tdc::User> m_current_user = nullptr;
    std::shared_ptr<tdc::UserDb> m_user_db =
        std::make_shared<tdc::UserDb>(DB_NAME);
    std::shared_ptr<tdc::TaskDb> m_task_db =
        std::make_shared<tdc::TaskDb>(DB_NAME);
    std::shared_ptr<tdc::MessageDb> m_message_db =
        std::make_shared<tdc::MessageDb>(DB_NAME);
    tdc::AuthenticationManager m_amanager{m_user_db};
    std::shared_ptr<tdu::IPrinter> m_printer = nullptr;
    std::shared_ptr<tdu::IUserInputHandler> m_input_handler = nullptr;
    tdu::Resource m_resource{};

    enum class UpdateEvent { UsernameUpdate, PasswordUpdate, RoleUpdate };

    Menu load_menu();
    std::shared_ptr<Page> load_main_menu() const;
    std::shared_ptr<Page> load_tasks_menu() const;
    std::shared_ptr<Page> load_settings_menu();

    bool user_update_event(UpdateEvent kind, tdc::User& user);
    void invalid_option_event() const;

    std::shared_ptr<tdc::User> get_current_user() { return m_current_user; }
};
}  // namespace twodo