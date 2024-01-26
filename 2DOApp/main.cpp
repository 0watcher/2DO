#include <2DOApp/app.hpp>
#include <Utils/util.hpp>
#include "fmt/core.h"

namespace tdu = twodoutils;
namespace td = twodo;


int main() {
    try {
        td::App::getInstance()->run();
    } catch (const std::runtime_error& e) {
        tdu::log_to_file(e.what(), ERR_LOGS_FILE_NAME);
        fmt::print(stderr, "{}", e.what());
    }
}