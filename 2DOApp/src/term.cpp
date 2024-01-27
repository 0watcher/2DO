#include "2DOApp/term.hpp"

#include <regex>
#include "Utils/result.hpp"
#include "fmt/core.h"

namespace twodo {
[[nodiscard]] tdu::Result<tdc::User, AuthErr> RegisterManager::singup() {
    auto username_ = String{};

    const auto username_input = [this, &username_]() {
        while (true) {
            m_printer->msg_print("username: ");
            const auto username = m_ihandler->get_input();

            const auto username_result = username_validation(username);

            if (!username_result) {
                m_printer->msg_print("Invalid name length!\n");
            } else {
                const auto name_exists =
                    m_udb->find_object_by_unique_column(username);

                if (!name_exists) {
                    username_ = username;
                    break;
                }

                m_printer->msg_print("User with this name already exists!\n");
            }
        }
    };

    const auto password_input =
        [this, &username_]() -> tdu::Result<tdc::User, AuthErr> {
        while (true) {
            m_printer->msg_print("password: ");
            const auto password = m_ihandler->get_input();

            const auto passwd_result = password_validation(password);
            if (!passwd_result) {
                switch (passwd_result.err()) {
                    case AuthErr::InvalidPassLength: {
                        m_printer->msg_print(
                            "Password must contain at least 8 characters and "
                            "max 20!\n");
                        break;
                    }
                    case AuthErr::MissingLowerCase: {
                        m_printer->msg_print(
                            "Password must contain at "
                            "least one lowercase "
                            "letter!\n");
                        break;
                    }
                    case AuthErr::MissingUpperCase: {
                        m_printer->msg_print(
                            "Password must contain at "
                            "least one uppercase "
                            "letter!\n");
                        break;
                    }
                    case AuthErr::MissingNumber: {
                        m_printer->msg_print(
                            "Password must contain at "
                            "least one number "
                            "letter!\n");
                        break;
                    }
                    case AuthErr::MissingSpecialCharacter: {
                        m_printer->msg_print(
                            "Password must contain at "
                            "least one special "
                            "character!\n");
                        break;
                    }
                }
            } else {
                tdc::Role usr_role{};

                if (m_udb->is_table_empty()) {
                    usr_role = tdc::Role::Admin;
                } else {
                    usr_role = tdc::Role::User;
                }

                auto user = tdc::User{username_, usr_role, password};
                if (!m_udb->add_object(user)) {
                    return tdu::Err(AuthErr::DbErr);
                }

                return tdu::Ok(std::move(user));
            }
        }
    };

    username_input();

    return password_input();
}

[[nodiscard]] tdu::Result<void, AuthErr> RegisterManager::username_validation(
    StringView username) const {
    if (username.length() < 1 || username.length() > 20) {
        return tdu::Err(AuthErr::InvalidNameLength);
    }

    return tdu::Ok();
}

[[nodiscard]] tdu::Result<void, AuthErr> RegisterManager::password_validation(
    const String& password) const {
    const std::regex upper_case_expression{"[A-Z]+"};
    const std::regex lower_case_expression{"[a-z]+"};
    const std::regex number_expression{"[0-9]+"};
    const std::regex special_char_expression{
        "[!@#$%^&*()_+\\-=\\[\\]{};:\\\",<.>/?]+"};

    if (password.length() <= 8 && password.length() > 20) {
        return tdu::Err(AuthErr::InvalidPassLength);
    }
    if (!std::regex_search(password, upper_case_expression)) {
        return tdu::Err(AuthErr::MissingUpperCase);
    }
    if (!std::regex_search(password, lower_case_expression)) {
        return tdu::Err(AuthErr::MissingLowerCase);
    }
    if (!std::regex_search(password, number_expression)) {
        return tdu::Err(AuthErr::MissingNumber);
    }
    if (!std::regex_search(password, special_char_expression)) {
        return tdu::Err(AuthErr::MissingSpecialCharacter);
    }

    return tdu::Ok();
}

[[nodiscard]] tdu::Result<tdc::User, AuthErr> AuthManager::login() {
    std::optional<tdc::User> user{};

    const auto username_input = [this, &user]() {
        while (true) {
            m_printer->msg_print("username: ");
            const auto username = m_ihandler->get_input();

            const auto result = m_udb->find_object_by_unique_column(username);
            if (!result) {
                m_printer->msg_print("User not found!\n");
            } else {
                user = result.unwrap();
                break;
            }
        }
    };

    const auto password_input = [this,
                                 &user]() -> tdu::Result<tdc::User, AuthErr> {
        unsigned int tries = 3;

        while (true) {
            if (tries <= 0) {
                return tdu::Err(AuthErr::AllTriesExhausted);
            }

            m_printer->msg_print("password: ");
            const String password = m_ihandler->get_input();
            if (tdu::hash(password) == user->password()) {
                return tdu::Ok(std::move(user.value()));
            }
            m_printer->msg_print("Invalid password!\n");
            tries--;
        }
    };

    username_input();

    return password_input();
}

[[nodiscard]] tdu::Result<void, AuthErr> AuthManager::auth_username() {
    m_printer->msg_print("username: ");
    String username = m_ihandler->get_input();

    auto result = m_udb->find_object_by_unique_column(username);
    if (!result) {
        return tdu::Err(AuthErr::UserNotFound);
    }

    return tdu::Ok();
}

[[nodiscard]] tdu::Result<tdc::User, AuthErr> AuthManager::auth_password(
    const String& username) {
    auto result = m_udb->find_object_by_unique_column(username);
    if (!result) {
        return tdu::Err(AuthErr::UserNotFound);
    }

    while (true) {
        m_printer->msg_print("password: ");
        String password = m_ihandler->get_input();

        if (password == result.unwrap().password()) {
            return tdu::Ok(std::move(result.unwrap()));
        }

        m_printer->msg_print("Invalid password!\n");
    }
}
}  // namespace twodo