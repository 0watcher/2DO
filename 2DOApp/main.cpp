#include <2DOApp/app.hpp>
#include <2DOCore/user.hpp>
#include <2DOCore/task.hpp>
#include <memory>

namespace td = twodo;
namespace tdc = twodocore;

int main() {
    try {
        //td::App(make_shared<tdc::User>(), make_unique<tdc::UserDb>(), make_unique<tdc::TaskDb>()).run();
    } catch (const std::runtime_error& e) {
        std::cerr << "Big Error: " << e.what() << '\n';
    }
}