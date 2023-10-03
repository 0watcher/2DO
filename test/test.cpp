
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <memory>
#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "result.hpp"
#include "user.hpp"
#include "utils.hpp"

using namespace twodo;
using namespace testing;
namespace fs = std::filesystem;

struct DbTest : Test
{
    const std::string db_path = "../../test/";
    const std::string db_name = "db";

    std::unique_ptr<Database> cut;

    void SetUp() override { cut = std::make_unique<Database>(db_path + db_name); }

    void TearDown() override
    {
        cut.reset();

        if (!fs::remove(db_path + db_name + ".db3"))
        {
            perror("Error deleting file");
        }
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
    String db_path = "../../test/db";
    auto udb = std::make_unique<UserDb>(db_path);

    EXPECT_TRUE(udb->is_empty());

    String username = "Papryk";
    auto user = User {username, Role::User, hash("Patryk!123")};
    auto result = udb->add_user(user);
    EXPECT_TRUE(result);

    auto user_from_db = udb->get_user(username);
    EXPECT_TRUE(user_from_db);

    user.set_id(user_from_db.value().get_id());
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
    if (!fs::remove(db_path + ".db3"))
    {
        perror("Error deleting file");
    }
}

class MockUserInputHandler : public IUserInputHandler
{
   public:
    MOCK_METHOD(String, get_input, (), (override));
};

class MockDisplayer : public IDisplayer
{
   public:
    MOCK_METHOD(void, msg_display, (const String& msg), (override));
    MOCK_METHOD(void, err_display, (const String& err), (override));
};

struct RegisterTest : Test
{
    String db_path = "../../test/db";
    MockUserInputHandler ih;
    MockDisplayer d;
    UserDb udb {db_path};
    std::unique_ptr<RegisterManager> cut;

    void SetUp() override { cut = std::make_unique<RegisterManager>(std::move(udb), ih, d); }

    void TearDown() override
    {
        cut.reset();

        if (!fs::remove(db_path + ".db3"))
        {
            perror("Error deleting file");
        }
    }
};

TEST_F(RegisterTest, ValidationTest)
{
    auto result = cut->username_validation("Patryk");
    EXPECT_TRUE(result);
    auto user = cut->password_validation("Patryk123!");
    EXPECT_TRUE(user);
}

TEST_F(RegisterTest, ChecksSingupOverallFunctionality)
{
    // int id = 1;
    // String username = "Patryk";
    // String hashed_password = hash("Patryk123!");
    // Role role = Role::Admin;
    // Ok<User, AuthErr>(User{id, username, role, hashed_password});

    // Expectation dplay1 = EXPECT_CALL(d, msg_display(_)).WillOnce(Return());
    // Expectation dplay2 = EXPECT_CALL(d, msg_display(_)).WillOnce(Return());

    // EXPECT_CALL(ih, get_input()).After(dplay1).WillOnce(Return("Patryk"));
    // EXPECT_CALL(ih, get_input()).After(dplay2).WillOnce(Return("Patryk123!"));
    // auto user_ = cut->singup();
    // EXPECT_TRUE(user_);
}
