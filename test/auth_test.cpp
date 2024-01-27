#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <filesystem>

#include <2DOApp/term.hpp>
#include <Utils/util.hpp>
#include "Utils/type.hpp"

constexpr StringView TEST_DB_PATH = "../../test/db.db3";

namespace fs = std::filesystem;
namespace td = twodo;
namespace tdu = twodoutils;

class MockUserInputHandler : public tdu::IUserInputHandler<String> {
  public:
    MOCK_METHOD(String, get_input, (), (const override));
};

class MockDisplayer : public tdu::IPrinter {
  public:
    MOCK_METHOD(void, msg_print, (StringView msg), (const override));
    MOCK_METHOD(void, err_print, (StringView err), (const override));
};

struct RegisterTest : testing::Test {
    std::shared_ptr<tdc::UserDb> udb =
        std::make_shared<tdc::UserDb>(TEST_DB_PATH);
    std::shared_ptr<MockUserInputHandler> ih =
        std::make_shared<MockUserInputHandler>();
    std::shared_ptr<MockDisplayer> d = std::make_shared<MockDisplayer>();
    std::unique_ptr<td::RegisterManager> cut =
        std::make_unique<td::RegisterManager>(udb, ih, d);

    void TearDown() override {
        cut.reset();
        udb.reset();
        ih.reset();
        d.reset();
        fs::path filepath = fs::path(TEST_DB_PATH);
        fs::remove(filepath);
    }
};

TEST_F(RegisterTest, ValidationTest) {
    auto result = cut->username_validation("Patryk");
    EXPECT_TRUE(result);

    auto result2 = cut->username_validation("");
    EXPECT_FALSE(result2);

    auto user = cut->password_validation("Patryk123!");
    EXPECT_TRUE(user);

    auto user2 = cut->password_validation("patryk");
    EXPECT_FALSE(user2);
}

TEST_F(RegisterTest, ChecksSingupOverallFunctionality) {
    EXPECT_CALL(*ih, get_input())
        .WillOnce(testing::Return("Patryk"))
        .WillOnce(testing::Return("Patryk123!"));

    tdc::User expectedUser{1, "Patryk", tdc::Role::Admin, "Patryk123!"};

    auto user_ = cut->singup();
    EXPECT_TRUE(user_);
    EXPECT_EQ(user_.unwrap(), expectedUser);
}

struct LoginTest : testing::Test {
    std::shared_ptr<tdc::UserDb> udb =
        std::make_shared<tdc::UserDb>(TEST_DB_PATH);
    std::shared_ptr<MockUserInputHandler> ih =
        std::make_shared<MockUserInputHandler>();
    std::shared_ptr<MockDisplayer> d = std::make_shared<MockDisplayer>();
    std::unique_ptr<td::AuthManager> am =
        std::make_unique<td::AuthManager>(udb, ih, d);
    String username = "SomeUser";
    String password = "VeryStrongPassword123!";
    tdc::User user = tdc::User{username, tdc::Role::User, password};

    void SetUp() override {
        EXPECT_TRUE(udb->add_object(user));
    }

    void TearDown() override {
        am.reset();
        udb.reset();
        ih.reset();
        d.reset();
        fs::path filepath = fs::path(TEST_DB_PATH);
        fs::remove(filepath);
    }
};

TEST_F(LoginTest, LoginCorrectness) {
    EXPECT_CALL(*ih, get_input())
        .WillOnce(testing::Return(username))
        .WillOnce(testing::Return(password));

    auto returned_user = am->login();
    EXPECT_TRUE(returned_user);
    EXPECT_EQ(returned_user.unwrap(), user);
}