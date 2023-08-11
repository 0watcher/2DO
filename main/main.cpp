#include "app.hpp"
#include "db.hpp"

using namespace twodo;

#include <string>

using namespace twodo;

int main()
{
    App app{};

    auto result = app.run();
    if(result.error() != ErrorCode::Ok)
    {
        return 1;   
    }
    return 0;
}