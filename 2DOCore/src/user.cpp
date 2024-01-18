#include "2DOCore/user.hpp"

#include <stdexcept>

#define USERS_TABLE "users"

namespace twodocore {
[[nodiscard]] String rtos(Role role) {
    switch (role) {
        case Role::Admin:
            return "Admin";
            break;
        case Role::User:
            return "User";
            break;
        default:
            throw std::logic_error("Invalid role enum!");
            break;
    }
}

[[nodiscard]] Role stor(const String& role_str) {
    static const std::map<std::string, Role> role_map = {
        {"User", Role::User}, {"Admin", Role::Admin}};

    const auto it = role_map.find(role_str);
    if (it != role_map.end()) {
        return it->second;
    } else {
        throw std::logic_error("Invalid role string!");
    }
}
}  // namespace twodocore