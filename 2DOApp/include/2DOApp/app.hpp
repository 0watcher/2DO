#pragma once

#include <memory>
#include <2DOCore/result.hpp>
#include <2DOCore/user.hpp>

namespace tdc = twodocore;

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
    tdc::Result<tdc::None, RunError> run();

   private:
    bool m_is_running = true;

    tdc::Result<int, InputError> get_valid_option() noexcept;
};

class Session
{
   public:
    std::shared_ptr<tdc::User> curr_user;
};

}  // namespace twodo