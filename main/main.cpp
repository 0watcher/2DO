#include "app.hpp"
#include "db.hpp"

using namespace twodo;

#include <string>
#include "result.hpp"

using namespace twodo;

int foo(int&& x)
{
    return x;
} 

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