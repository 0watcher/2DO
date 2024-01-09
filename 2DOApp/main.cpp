#include <2DOApp/app.hpp>

using namespace std;
using namespace twodo;

int main() {
    try {
        App{}.run();
    } catch (const std::runtime_error& e) {
        std::cerr << "Big Error: " << e.what() << '\n';
    }
}