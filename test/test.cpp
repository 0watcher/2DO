#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <2DOApp/term.hpp>
#include <2DOCore/result.hpp>
#include <2DOCore/task.hpp>
#include <2DOCore/user.hpp>
#include <2DOCore/utils.hpp>
#include <chrono>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>

using namespace twodocore;
using namespace twodo;
using namespace testing;
namespace fs = std::filesystem;

#define TEST_DB_PATH "../../test/db"

TEST(ResultTest, OkWithValue)
{
    auto result = Ok<int, None>(42);
    ASSERT_TRUE(result);
    EXPECT_EQ(result.value(), 42);
}

TEST(ResultTest, OkWithConstValue)
{
    const int value = 42;
    auto result = Ok<int, None>(value);
    ASSERT_TRUE(result);
    EXPECT_EQ(result.value(), value);
}

TEST(ResultTest, ErrWithError)
{
    auto result = Err<int, std::string>("Error message");
    ASSERT_FALSE(result);
    EXPECT_EQ(result.err(), "Error message");
}

TEST(ResultTest, Accessors)
{
    auto result = Ok<int, std::string>(42);
    ASSERT_TRUE(result);
    EXPECT_EQ(result.value(), 42);

    result = Err<int, std::string>("Error message");
    ASSERT_FALSE(result);
    EXPECT_EQ(result.err(), "Error message");
}

struct DbTest : Test
{
    std::unique_ptr<Database> cut = std::make_unique<Database>(TEST_DB_PATH);

    void TearDown() override
    {
        cut.reset();
        fs::remove(TEST_DB_PATH ".db3");
    }
};

TEST_F(DbTest, CheckDbFunctionalities)
{
    const std::string name = "patryk";
    const std::string password = "patryk123";
    {
        auto ct_result =
            cut->create_table("users", {{"id", "INT"}, {"name", "TEXT"}, {"password", "TEXT"}});
        if (!ct_result)
        {
            FAIL() << "CreateTableFailure";
        }
        auto i_result =
            cut->insert_data("users", {{"id", "1"}, {"name", name}, {"password", password}});
        if (!i_result)
        {
            FAIL() << "InsertTableFailure";
        }
        auto s_result = cut->select_data("users", {"name", "password"}, {"id", "1"});
        if (!s_result)
        {
            FAIL() << "SelectDataFailure";
        }

        auto s_name = s_result.value()[0];
        auto s_password = s_result.value()[1];

        ASSERT_EQ(s_name, name);
        ASSERT_EQ(s_password, password);

        const std::string new_name = "dupa";

        auto u_result = cut->update_data("users", {"name", new_name}, {"id", "1"});
        if (!u_result)
        {
            FAIL() << "UpdateFailure";
        }

        auto ns_result = cut->select_data("users", {"name", "password"}, {"id", "1"});
        if (!ns_result)
        {
            FAIL() << "SelectDataFailure";
        }

        auto updated_name = ns_result.value()[0];

        ASSERT_EQ(updated_name, new_name);
    }
}

TEST(UserDbTest, ChecksOverallUserDbFunctionality)
{
    auto udb = std::make_unique<UserDb>(TEST_DB_PATH);

    EXPECT_TRUE(udb->is_empty());

    String username = "Papryk";
    auto user = User {username, Role::User, hash("Patryk!123")};
    auto result = udb->add_user(user);
    EXPECT_TRUE(result);

    auto user_from_db = udb->get_user(username);
    EXPECT_TRUE(user_from_db);

    auto id = udb->get_user_id(user.get_username());
    EXPECT_TRUE(id);
    user.set_id(id.value());
    EXPECT_EQ(user_from_db.value(), user);

    user.set_username("Dupa");
    user.set_role(Role::Admin);
    user.set_password(hash("Dupa123!"));
    auto result2 = udb->update_data(user);
    EXPECT_TRUE(result2);
    auto user_from_db2 = udb->get_user(user.get_id());
    EXPECT_TRUE(user_from_db2);
    EXPECT_EQ(user_from_db2.value(), user);

    auto result3 = udb->delete_user(user.get_id());
    EXPECT_TRUE(result3);

    udb.reset();
    fs::remove(TEST_DB_PATH ".db3");
}

class MockUserInputHandler : public IUserInputHandler<String>
{
   public:
    MOCK_METHOD(String, get_input, (), (override));
};

class MockDisplayer : public IDisplayer
{
   public:
    MOCK_METHOD(void, msg_display, (std::string_view msg), (override));
    MOCK_METHOD(void, err_display, (std::string_view err), (override));
};

struct RegisterTest : Test
{
    std::shared_ptr<UserDb> udb = std::make_shared<UserDb>(TEST_DB_PATH);
    std::shared_ptr<MockUserInputHandler> ih = std::make_shared<MockUserInputHandler>();
    std::shared_ptr<MockDisplayer> d = std::make_shared<MockDisplayer>();
    std::unique_ptr<RegisterManager> cut = std::make_unique<RegisterManager>(udb, ih, d);

    void TearDown() override
    {
        cut.reset();
        udb.reset();
        ih.reset();
        d.reset();
        fs::remove(TEST_DB_PATH ".db3");
    }
};

TEST_F(RegisterTest, ValidationTest)
{
    auto result = cut->username_validation("Patryk");
    EXPECT_TRUE(result);

    auto result2 = cut->username_validation("");
    EXPECT_FALSE(result2);

    auto user = cut->password_validation("Patryk123!");
    EXPECT_TRUE(user);

    auto user2 = cut->password_validation("patryk");
    EXPECT_FALSE(user2);
}

TEST_F(RegisterTest, ChecksSingupOverallFunctionality)
{
    EXPECT_CALL(*ih, get_input()).WillOnce(Return("Patryk")).WillOnce(Return("Patryk123!"));

    User expectedUser {1, "Patryk", Role::Admin, hash("Patryk123!")};

    auto user_ = cut->singup();
    EXPECT_TRUE(user_);
    EXPECT_EQ(user_.value(), expectedUser);
}

struct LoginTest : Test
{
    std::shared_ptr<UserDb> udb = std::make_shared<UserDb>(TEST_DB_PATH);
    std::shared_ptr<MockUserInputHandler> ih = std::make_shared<MockUserInputHandler>();
    std::shared_ptr<MockDisplayer> d = std::make_shared<MockDisplayer>();
    std::unique_ptr<AuthManager> am = std::make_unique<AuthManager>(udb, ih, d);
    String username = "SomeUser";
    String password = "VeryStrongPassword123!";
    User added_user = User {username, Role::User, hash(password)};

    void SetUp() override
    {
        udb->add_user(added_user);
        auto id = udb->get_user_id(username);
        added_user.set_id(id.value());
    }

    void TearDown() override
    {
        am.reset();
        udb.reset();
        ih.reset();
        d.reset();
        fs::remove(TEST_DB_PATH ".db3");
    }
};

TEST_F(LoginTest, LoginCorrectness)
{
    EXPECT_CALL(*ih, get_input()).WillOnce(Return(username)).WillOnce(Return(password));

    auto user = am->login();
    EXPECT_TRUE(user);
    EXPECT_EQ(user.value(), added_user);
}

TEST_F(LoginTest, AuthCorrectness) {}

TEST(TaskDbTest, CheckTaskDbOverallCorrectness)
{
    auto db_path = String {"../../test/db"};
    std::unique_ptr<TaskDb> tdb = std::make_unique<TaskDb>(db_path);
    auto task = Task {"sometopic", "somecontent", give_date(), give_date(5), 1, 2, 0, false};

    EXPECT_TRUE(tdb->add_task(task));
    auto db_task = tdb->get_task(task.get_id());
    EXPECT_TRUE(db_task);
    EXPECT_EQ(db_task.value(), task);
    tdb.reset();
    fs::remove(db_path + ".db3");
}