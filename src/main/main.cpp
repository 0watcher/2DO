#include "app.hpp"
#include "result.hpp"

using namespace twodo;

int main()
{
    App app{};

    auto result = app.run();
    if(!result)
    {
        return EXIT_FAILURE;
    }
}