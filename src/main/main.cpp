#include "app.hpp"
#include "result.hpp"

#include <string>

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