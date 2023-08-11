#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>

#include "result.hpp"
#include "user.hpp"
#include "utils.hpp"

using namespace twodo;
using namespace testing;

struct LoginTest : Test
{
    Login l{};
};

TEST_F(LoginTest, NicknameTest)
{
    std::string nicknm1 = "";
    std::string nicknm2 = "nickname";

    auto result1 = l.nickname(nicknm1);

    EXPECT_EQ(result1.error(), ErrorCode::IncorrectNickname);

    auto result2 = l.nickname(nicknm2);

    EXPECT_EQ(result2.error(), ErrorCode::Ok);
}

TEST_F(LoginTest, PasswordTest)
{
    std::string passwd1 = "passwd";
    std::string passwd2 = "password";
    std::string passwd3 = "Pass_word1234";

    auto result1 = l.password(passwd1);

    EXPECT_EQ(result1.error(), ErrorCode::IncorrectPassword);

    auto result2 = l.password(passwd2);

    EXPECT_EQ(result2.error(), ErrorCode::IncorrectPassword);

    auto result3 = l.password(passwd3);

    EXPECT_EQ(result3.error(), ErrorCode::Ok);
    EXPECT_EQ(result3.value(), hash(passwd3).value());
}   

TEST(UserDb, UserExistenceInDB)
{

}

TEST(UserManagerTest, AddUser)
{

}

TEST(UserManagerTest, DeleteUser)
{

}

TEST(UserManagerTest, ModifyUserData)
{

}

