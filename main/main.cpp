#include "app.hpp"

using namespace twodo;

int main()
{
    App app{};

    auto result = app.run();
    if(result.m_err != ErrorCode::ok)
    {
        
    }
}