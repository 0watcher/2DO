#include <2DOApp/app.hpp>
#include <2DOCore/user.hpp>
#include <2DOCore/task.hpp>
#include <memory>

namespace td = twodo;

int main() {
    try {
        td::App::getInstance()->run();
    } catch (const std::runtime_error& e) {
        fmt::print(stderr, "{}", e.what());
    }
}