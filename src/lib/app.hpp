#pragma once

#include "result.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <cstdlib>
#endif

namespace twodo
{
    enum class RunError
    {
        ExitFailure
    };

    enum class InputError
    {
        InvalidInput,
    };

    class App
    {
    public:
        Result<None, RunError> run();

    private:
        bool m_is_running = true;
        bool m_in_task_menu = true;

        void clearConsole() noexcept;
        Result<int, InputError> get_valid_option() noexcept;
    };
}  // namespace twodo