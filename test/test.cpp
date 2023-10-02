
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <memory>
#include <string>

#include "gmock/gmock.h"
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
    MockUserInputHandler ih;
    MockDisplayer d;
    UserDb udb{"../../test/db"};
    RegisterManager cut{std::move(udb), ih, d};
};

TEST_F(RegisterTest, Some)
{
    auto result = cut.username_validation("Patryk");
    EXPECT_TRUE(result);
    auto user = cut.password_validation("Patryk123!");
    EXPECT_TRUE(user);
}
