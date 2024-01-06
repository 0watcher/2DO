#include <2DOApp/app.hpp>
#include <2DOApp/term.hpp>
#include <2DOCore/result.hpp>
#include <2DOCore/user.hpp>
#include <2DOCore/utils.hpp>
#include <memory>
#include <variant>

#include "fmt/core.h"

using namespace twodo;
using namespace twodocore;

#include <memory>
#include <string>

class MockUserInputHandler : public IUserInputHandler<String>
{
   public:
    String get_input() override
    {
        auto input = std::string();
        std::getline(std::cin, input);
        return input;
    }
};

class MockDisplayer : public IDisplayer
{
   public:
    void msg_display(std::string_view msg) override { std::cout << msg; }

    void err_display(std::string_view err) override { std::cerr << err; }
};

#include <filesystem>
namespace fs = std::filesystem;

int main()
{
    // App app{};

    // auto result = app.run();
    // if(!result)
    // {
    //     return EXIT_FAILURE;
    // }

    // auto db_path =  String("../../test/db");
    // std::shared_ptr<UserDb> udb = std::make_shared<UserDb>(db_path);
    // std::shared_ptr<MockUserInputHandler> ih = std::make_shared<MockUserInputHandler>();
    // std::shared_ptr<MockDisplayer> d = std::make_shared<MockDisplayer>();

    // if (!udb || !ih || !d)
    // {
    //     std::cerr << "Failed to initialize one or more objects." << std::endl;
    //     return EXIT_FAILURE;
    // }

    // std::cout << "Register:\n";
    // auto rm = std::make_unique<RegisterManager>(udb, ih, d);
    // auto result = rm->singup();
    // if (!result)
    // {
    //     std::cerr << "Something goes wrong!";
    // }

    // rm.reset();
    // udb.reset();
    // ih.reset();
    // d.reset();

    // std::shared_ptr<UserDb> udb2 = std::make_shared<UserDb>(db_path);
    // std::shared_ptr<MockUserInputHandler> ih2 = std::make_shared<MockUserInputHandler>();
    // std::shared_ptr<MockDisplayer> d2 = std::make_shared<MockDisplayer>();

    // std::cout << "Login:\n";
    // auto am = std::make_unique<AuthManager>(udb2, ih2, d2);
    // auto user = am->login();
    // if (!user)
    // {
    //     std::cerr << "Something goes wrong!";
    // }

    // am.reset();
    // udb2.reset();
    // ih2.reset();
    // d2.reset();
    // fs::remove(db_path + ".db3");

    std::shared_ptr<Page<String>> main = std::make_shared<Page<String>>(
        []()
        {
            fmt::print(
                "Main Menu:\n"
                "[1] Tasks\n"
                "[2] Users\n"
                "[3] Settings\n"
                "[0] Exit\n"
                "-> ");
        });

    MockDisplayer md {};
    MockUserInputHandler muih {};

    Menu m {main, md, muih};

    std::shared_ptr<Page<String>> s = std::make_shared<Page<String>>(
        []()
        {
            fmt::print(
                "Settings:\n"
                "[1] s1\n"
                "[2] s2\n"
                "[3] s3\n"
                "[0] Back\n"
                "-> ");
        });
    main->add_child("1", s);

    m.run("0");
}