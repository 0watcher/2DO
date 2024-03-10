#include <memory>

#include <gtest/gtest.h>

#include <2DOCore/user.hpp>
#include <Utils/type.hpp>

namespace tdc = twodocore;

constexpr StringView TEST_DB_PATH = ":memory:";

TEST(AuthTest, CheckAuthManagerFunctionalities) {
    const auto udb = std::make_shared<tdc::UserDb>(TEST_DB_PATH);
    const tdc::AuthenticationManager am{udb};

    EXPECT_FALSE(am.username_validation(""));
    EXPECT_TRUE(am.username_validation("user"));

    EXPECT_FALSE(am.password_validation(""));
    EXPECT_FALSE(am.password_validation("haslo"));
    EXPECT_FALSE(am.password_validation("Haslo"));
    EXPECT_FALSE(am.password_validation("Haslo123"));
    EXPECT_TRUE(am.password_validation("Haslo123!"));
}