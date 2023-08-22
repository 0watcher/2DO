#include "app.hpp"
#include "db.hpp"
#include "result.hpp"

#include <string>

using namespace twodo;

int main()
{
    App app{};

    auto result = app.run();
    if(!result)
    {
        return 1;   
    }
    return 0;
}