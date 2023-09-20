
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <string>

#include "database.hpp"
#include "result.hpp"
#include "user.hpp"
#include "utils.hpp"

using namespace testing;
using namespace twodo;
namespace fs = std::filesystem;

#include <memory>

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
// struct LoginTest : Test
// {
//     AuthManager cut {};
// };

// TEST_F(LoginTest, NicknameTest)
// {
//     std::string nicknm1 = "";
//     std::string nicknm2 = "nickname";

//     auto result1 = cut.auth_nickname(nicknm1);

//     EXPECT_EQ(result1.error(), AuthErr::IncorrectNickname);

//     auto result2 = cut.auth_nickname(nicknm2);

//     EXPECT_TRUE(result2);
// }

// TEST_F(LoginTest, PasswordTest)
// {
//     std::string passwd1 = "passwd";
//     std::string passwd2 = "password";
//     std::string passwd3 = "Pass_word1234";

//     auto result1 = cut.auth_password(passwd1);

//     EXPECT_EQ(result1.error(), AuthErr::IncorrectPassword);

//     auto result2 = cut.auth_password(passwd2);

//     EXPECT_EQ(result2.error(), AuthErr::IncorrectPassword);

//     auto result3 = cut.auth_password(passwd3);

//     EXPECT_TRUE(result3);
//     EXPECT_EQ(result3.value(), hash(passwd3).value());
// }