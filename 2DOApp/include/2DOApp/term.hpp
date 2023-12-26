#include <2DOCore/result.hpp>
#include <2DOCore/user.hpp>
#include <2DOCore/utils.hpp>

namespace tdc = twodocore;

namespace twodo
{
enum class AuthErr
{
    InvalidNameLength = 1,
    AlreadyExistingName,
    PasswordTooShort,
    MissingUpperCase,
    MissingLowerCase,
    MissingNumber,
    MissingSpecialCharacter,
    UserNotFound,
    AllTriesExhausted,
    DbErr,
};

class RegisterManager
{
   public:
    RegisterManager(std::shared_ptr<tdc::UserDb> udb, std::shared_ptr<tdc::IUserInputHandler> ihandler,
                    std::shared_ptr<tdc::IDisplayer> idisplayer)
        : m_udb {udb}, m_ihandler {ihandler}, m_idisplayer {idisplayer}
    {
    }

    [[nodiscard]] tdc::Result<tdc::User, AuthErr> singup();
    [[nodiscard]] tdc::Result<tdc::None, AuthErr> username_validation(std::string_view username) const;
    [[nodiscard]] tdc::Result<tdc::None, AuthErr> password_validation(const String& password) const;

   private:
    std::shared_ptr<tdc::UserDb> m_udb;
    std::shared_ptr<tdc::IUserInputHandler> m_ihandler;
    std::shared_ptr<tdc::IDisplayer> m_idisplayer;
};

class AuthManager
{
   public:
    AuthManager(std::shared_ptr<tdc::UserDb> udb, std::shared_ptr<tdc::IUserInputHandler> ihandler,
                std::shared_ptr<tdc::IDisplayer> idisplayer)
        : m_udb {udb}, m_ihandler {ihandler}, m_idisplayer {idisplayer}
    {
    }

    [[nodiscard]] tdc::Result<tdc::User, AuthErr> login();
    [[nodiscard]] tdc::Result<tdc::None, AuthErr> auth_username();
    [[nodiscard]] tdc::Result<tdc::User, AuthErr> auth_password(const String& username);

   private:
    std::shared_ptr<tdc::UserDb> m_udb;
    std::shared_ptr<tdc::IUserInputHandler> m_ihandler;
    std::shared_ptr<tdc::IDisplayer> m_idisplayer;
};
}  // namespace app