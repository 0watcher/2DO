#include <memory>
#include <variant>

#include "app.hpp"
#include "result.hpp"
#include "user.hpp"

using namespace twodo;

#include <memory>
#include <string>

#include "user.cpp"
#include "utils.hpp"

class MockUserInputHandler : public IUserInputHandler
{
   public:
    String get_input() override
    {
        auto input = std::string();
        std::cin >> input;
        //std::getline(std::cin, input);
        return input;
    }
};

class MockDisplayer : public IDisplayer
{
   public:
    void msg_display(std::string_view msg) override { std::cout << msg << std::endl; }

    void err_display(std::string_view err) override { std::cerr << err << std::endl; }
};

int main()
{   
    // App app{};

    // auto result = app.run();
    // if(!result)
    // {
    //     return EXIT_FAILURE;
    // }

    std::shared_ptr<UserDb> udb = std::make_shared<UserDb>("../../../test/db");
    std::shared_ptr<MockUserInputHandler> ih = std::make_shared<MockUserInputHandler>();
    std::shared_ptr<MockDisplayer> d = std::make_shared<MockDisplayer>();

    if (!udb || !ih || !d) {
    std::cerr << "Failed to initialize one or more objects." << std::endl;
    return EXIT_FAILURE;
}

    RegisterManager rm {udb , ih, d};
    auto result = rm.singup();
    if(!result)
    {
        std::cerr << "Something goes wrong!";
    }
}