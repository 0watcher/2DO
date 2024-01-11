#include "2DOApp/term.hpp"

#include <regex>

namespace twodo {
[[nodiscard]] tdl::Result<tdc::User, AuthErr> RegisterManager::singup() {
    auto username_ = String{};

    const auto username_input = [this, &username_]() {
        while (true) {
            m_idisplayer->msg_display("username: ");
            const auto username = m_ihandler->get_input();
            const auto username_result = username_validation(username);

            if (!username_result) {
                m_idisplayer->err_display("Invalid name length!\n");
            } else {
                const auto name_exists = m_udb->get_user(username);
                if (!name_exists) {
                    username_ = username;
                    break;
                }
                m_idisplayer->err_display(
                    "User with this name already exists!\n");
            }
        }
    };

    const auto password_input = [this, &username_]() {
        while (true) {
            m_idisplayer->msg_display("password: ");
            const auto password = m_ihandler->get_input();

            const auto passwd_result = password_validation(password);
            if (!passwd_result) {
                switch (passwd_result.err()) {
                    case AuthErr::PasswordTooShort: {
                        m_idisplayer->err_display("Password is too short!\n");
                        break;
                    }
                    case AuthErr::MissingLowerCase: {
                        m_idisplayer->err_display(
                            "Password must contain at "
                            "least one lowercase "
                            "letter!\n");
                        break;
                    }
                    case AuthErr::MissingUpperCase: {
                        m_idisplayer->err_display(
                            "Password must contain at "
                            "least one uppercase "
                            "letter!\n");
                        break;
                    }
                    case AuthErr::MissingNumber: {
                        m_idisplayer->err_display(
                            "Password must contain at "
                            "least one number "
                            "letter!\n");
                        break;
                    }
                    case AuthErr::MissingSpecialCharacter: {
                        m_idisplayer->err_display(
                            "Password must contain at "
                            "least one special "
                            "character!\n");
                        break;
                    }
                }
            } else {
                tdc::Role usr_role{};
                if (m_udb->is_empty()) {
                    usr_role = tdc::Role::Admin;
                } else {
                    usr_role = tdc::Role::User;
                }
                auto user = tdc::User{username_, usr_role, tdl::hash(password)};
                m_udb->add_user(user);

                return tdl::Ok<tdc::User, AuthErr>(user);
            }
        }
    };

    username_input();

    return password_input();
}

[[nodiscard]] tdl::Result<tdl::None, AuthErr>
RegisterManager::username_validation(StringView username) const {
    if (username.length() < 1 || username.length() > 20) {
        return tdl::Err<tdl::None, AuthErr>(AuthErr::InvalidNameLength);
    }
    return tdl::Ok<tdl::None, AuthErr>({});
}

[[nodiscard]] tdl::Result<tdl::None, AuthErr>
RegisterManager::password_validation(const String& password) const {
    const std::regex upper_case_expression{"[A-Z]+"};
    const std::regex lower_case_expression{"[a-z]+"};
    const std::regex number_expression{"[0-9]+"};
    const std::regex special_char_expression{
        "[!@#$%^&*()_+\\-=\\[\\]{};:\\\",<.>/?]+"};

    if (password.length() < 8) {
        return tdl::Err<tdl::None, AuthErr>(AuthErr::PasswordTooShort);
    }
    if (!std::regex_search(password, upper_case_expression)) {
        return tdl::Err<tdl::None, AuthErr>(AuthErr::MissingUpperCase);
    }
    if (!std::regex_search(password, lower_case_expression)) {
        return tdl::Err<tdl::None, AuthErr>(AuthErr::MissingLowerCase);
    }
    if (!std::regex_search(password, number_expression)) {
        return tdl::Err<tdl::None, AuthErr>(AuthErr::MissingNumber);
    }
    if (!std::regex_search(password, special_char_expression)) {
        return tdl::Err<tdl::None, AuthErr>(AuthErr::MissingSpecialCharacter);
    }

    return tdl::Ok<tdl::None, AuthErr>({});
}

[[nodiscard]] tdl::Result<tdc::User, AuthErr> AuthManager::login() {
    std::optional<tdc::User> user{};

    const auto username_input = [this, &user]() {
        while (true) {
            m_idisplayer->msg_display("username: ");
            const auto username = m_ihandler->get_input();
            const auto result = m_udb->get_user(username);
            if (!result) {
                m_idisplayer->err_display("User not found!\n");
            } else {
                user = result.value();
                break;
            }
        }
    };

    const auto password_input = [this, &user]() {
        int tries = 3;

        while (true) {
            if (tries <= 0) {
                return tdl::Err<tdc::User, AuthErr>(AuthErr::AllTriesExhausted);
            }

            m_idisplayer->msg_display("password: ");
            const auto password = m_ihandler->get_input();
            if (tdl::hash(password) == user->password()) {
                return tdl::Ok<tdc::User, AuthErr>(std::move(user.value()));
            }
            m_idisplayer->err_display("Invalid password!\n");
            tries--;
        }
    };

    username_input();

    return password_input();
}

[[nodiscard]] tdl::Result<tdl::None, AuthErr> AuthManager::auth_username() {
    m_idisplayer->msg_display("username: ");
    String username = m_ihandler->get_input();
    auto result = m_udb->get_user(username);
    if (!result) {
        return tdl::Err<tdl::None, AuthErr>(AuthErr::UserNotFound);
    }
    return tdl::Ok<tdl::None, AuthErr>({});
}

[[nodiscard]] tdl::Result<tdc::User, AuthErr> AuthManager::auth_password(
    const String& username) {
    auto result = m_udb->get_user(username);
    if (!result) {
        return tdl::Err<tdc::User, AuthErr>(AuthErr::UserNotFound);
    }
    while (true) {
        m_idisplayer->msg_display("password: ");
        String password = m_ihandler->get_input();
        if (password == result.value().password()) {
            return tdl::Ok<tdc::User, AuthErr>(std::move(result.value()));
        }
        m_idisplayer->err_display("Invalid password!\n");
    }
}
}  // namespace twodo