#include <2DOApp/app.hpp>
#include <Utils/util.hpp>
#include "Utils/type.hpp"
#include "fmt/core.h"

#include <2DOApp/term.hpp>
namespace tdu = twodoutils;
namespace td = twodo;


class UserInput : public tdu::IUserInputHandler<String> {
  public:
    String get_input() const override {
        auto input = String();
        std::getline(std::cin, input);
        return input;
    }
};

class MsgDisplayer : public tdu::IPrinter {
  public:
    void msg_print(StringView msg) const override { fmt::println("{}", msg); }

    void err_print(StringView err) const override {
        fmt::println(stderr, "{}", err);
    }
};

int main() {
    // try {
    //     td::App::getInstance()->run();
    // } catch (const std::runtime_error& e) {
    //     tdu::log_to_file(e.what(), ERR_LOGS_FILE_NAME);
    //     fmt::print(stderr, "{}", e.what());
    // }

    const auto p1 = tdu::hash("Patryk123!");
    const auto p2 = tdu::hash("Patryk123!");
    fmt::println("{}, {}", p1, p2);

    constexpr StringView TEST_DB_PATH = "../../test/db.db3";

    std::shared_ptr<tdc::UserDb> user_db =
        std::make_shared<tdc::UserDb>(TEST_DB_PATH);

    tdc::User user{"patryk", tdc::Role::Admin, "Patryk123!"};
    user_db->add_object(user);

    const auto user_ = user_db->get_object(user.id()).unwrap();

    fmt::println("{}, {}, {}, {}", user.id(), user.username(), user.role<String>(), user.password());
    fmt::println("{}, {}, {}, {}", user_.id(), user_.username(), user_.role<String>(), user_.password());

    std::shared_ptr<UserInput> ui =
        std::make_shared<UserInput>();
        std::shared_ptr<MsgDisplayer> md =
        std::make_shared<MsgDisplayer>();

    td::AuthManager am{user_db, ui, md};
    const auto result = am.login();
    fmt::println("{}", result.is_ok());
}