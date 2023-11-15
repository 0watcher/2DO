#pragma once

#include <memory>

#include "result.hpp"
#include "user.hpp"
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

    Result<int, InputError> get_valid_option() noexcept;
};

class Session
{
   public:
    std::shared_ptr<User> curr_user;
};

}  // namespace twodo