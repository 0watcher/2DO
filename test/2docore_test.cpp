#include <2DOApp/term.hpp>
#include <2DOCore/task.hpp>
#include <2DOCore/user.hpp>
#include <Utils/database.hpp>
#include <Utils/result.hpp>
#include <Utils/type.hpp>
#include <Utils/util.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <filesystem>
#include <memory>

namespace tdc = twodocore;
namespace tdl = twodoutils;
namespace td = twodo;
namespace fs = std::filesystem;

constexpr const char* TEST_DB_PATH = "../../test/db";
constexpr const char* DB_FILE_EXT = ".db3";

struct DbTest : testing::Test {
    std::unique_ptr<tdl::Database> cut =
        std::make_unique<tdl::Database>(TEST_DB_PATH);

    void TearDown() override {
        cut.reset();
        fs::path filepath = fs::path(TEST_DB_PATH) / String(DB_FILE_EXT);
    fs:
        remove(filepath);
    }
};

TEST_F(DbTest, CheckDbFunctionalities) {
    const String name = "patryk";
    const String password = "patryk123";
    {
        auto ct_result = cut->create_table(
            "users", {{"id", "INT"}, {"name", "TEXT"}, {"password", "TEXT"}});
        if (!ct_result) {
            FAIL() << "CreateTableFailure";
        }
        auto i_result = cut->insert_data(
            "users", {{"id", "1"}, {"name", name}, {"password", password}});
        if (!i_result) {
            FAIL() << "InsertTableFailure";
        }
        auto s_result =
            cut->select_data("users", {"name", "password"}, {"id", "1"});
        if (!s_result) {
            FAIL() << "SelectDataFailure";
        }

        auto s_name = s_result.value()[0];
        auto s_password = s_result.value()[1];

        ASSERT_EQ(s_name, name);
        ASSERT_EQ(s_password, password);

        const String new_name = "dupa";

        auto u_result =
            cut->update_data("users", {"name", new_name}, {"id", "1"});
        if (!u_result) {
            FAIL() << "UpdateFailure";
        }

        auto ns_result =
            cut->select_data("users", {"name", "password"}, {"id", "1"});
        if (!ns_result) {
            FAIL() << "SelectDataFailure";
        }

        auto updated_name = ns_result.value()[0];

        ASSERT_EQ(updated_name, new_name);
    }
}

TEST(UserDbTest, ChecksOverallUserDbFunctionality) {
    auto udb = std::make_unique<tdc::UserDb>(TEST_DB_PATH);

    EXPECT_TRUE(udb->is_empty());

    String username = "Papryk";
    auto user = tdc::User{username, tdc::Role::User, tdl::hash("Patryk!123")};
    auto result = udb->add_user(user);
    EXPECT_TRUE(result);

    auto user_from_db = udb->get_user(username);
    EXPECT_TRUE(user_from_db);

    auto id = udb->get_user_id(user.username());
    EXPECT_TRUE(id);
    user.set_id(id.value());
    EXPECT_EQ(user_from_db.value(), user);

    user.set_username("Dupa");
    user.set_role(tdc::Role::Admin);
    user.set_password(tdl::hash("Dupa123!"));
    auto result2 = udb->update_data(user);
    EXPECT_TRUE(result2);
    auto user_from_db2 = udb->get_user(user.id());
    EXPECT_TRUE(user_from_db2);
    EXPECT_EQ(user_from_db2.value(), user);

    auto result3 = udb->delete_user(user.id());
    EXPECT_TRUE(result3);

    udb.reset();
    fs::path filepath = fs::path(TEST_DB_PATH) / String(DB_FILE_EXT);
fs:
    remove(filepath);
}

class MockUserInputHandler : public tdl::IUserInputHandler<String> {
  public:
    MOCK_METHOD(String, get_input, (), (override));
};

class MockDisplayer : public tdl::IDisplayer {
  public:
    MOCK_METHOD(void, msg_display, (std::string_view msg), (override));
    MOCK_METHOD(void, err_display, (std::string_view err), (override));
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
        fs::path filepath = fs::path(TEST_DB_PATH) / String(DB_FILE_EXT);
    fs:
        remove(filepath);
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

    tdc::User expectedUser{1, "Patryk", tdc::Role::Admin,
                           tdl::hash("Patryk123!")};

    auto user_ = cut->singup();
    EXPECT_TRUE(user_);
    EXPECT_EQ(user_.value(), expectedUser);
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
    tdc::User added_user =
        tdc::User{username, tdc::Role::User, tdl::hash(password)};

    void SetUp() override {
        udb->add_user(added_user);
        auto id = udb->get_user_id(username);
        added_user.set_id(id.value());
    }

    void TearDown() override {
        am.reset();
        udb.reset();
        ih.reset();
        d.reset();
        fs::path filepath = fs::path(TEST_DB_PATH) / String(DB_FILE_EXT);
    fs:
        remove(filepath);
    }
};

TEST_F(LoginTest, LoginCorrectness) {
    EXPECT_CALL(*ih, get_input())
        .WillOnce(testing::Return(username))
        .WillOnce(testing::Return(password));

    auto user = am->login();
    EXPECT_TRUE(user);
    EXPECT_EQ(user.value(), added_user);
}

TEST_F(LoginTest, AuthCorrectness) {}

TEST(TaskDbTest, CheckTaskDbOverallCorrectness) {
    std::unique_ptr<tdc::TaskDb> tdb =
        std::make_unique<tdc::TaskDb>(TEST_DB_PATH);
    auto task = tdc::Task{
        "sometopic", "somecontent", tdl::give_date(0), tdl::give_date(5), 1, 2,
        0,           false};

    EXPECT_TRUE(tdb->add_task(task));
    auto db_task = tdb->get_task(task.id());
    EXPECT_TRUE(db_task);
    EXPECT_EQ(db_task.value(), task);
    tdb.reset();
    fs::path filepath = fs::path(TEST_DB_PATH) / String(DB_FILE_EXT);
fs:
    remove(filepath);
}