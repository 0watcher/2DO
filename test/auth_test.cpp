#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <2DOApp/term.hpp>

// // class MockUserInputHandler : public tdl::IUserInputHandler<String> {
//   public:
//     MOCK_METHOD(String, get_input, (), (const override));
// };

// class MockDisplayer : public tdl::IPrinter {
//   public:
//     MOCK_METHOD(void, msg_print, (std::string_view msg), (const override));
//     MOCK_METHOD(void, err_print, (std::string_view err), (const override));
// };

// struct RegisterTest : testing::Test {
//     std::shared_ptr<tdc::UserDb> udb =
//         std::make_shared<tdc::UserDb>(TEST_DB_PATH);
//     std::shared_ptr<MockUserInputHandler> ih =
//         std::make_shared<MockUserInputHandler>();
//     std::shared_ptr<MockDisplayer> d = std::make_shared<MockDisplayer>();
//     std::unique_ptr<td::RegisterManager> cut =
//         std::make_unique<td::RegisterManager>(udb, ih, d);

//     void TearDown() override {
//         cut.reset();
//         udb.reset();
//         ih.reset();
//         d.reset();
//         fs::path filepath = fs::path(TEST_DB_PATH);
//     fs:
//         remove(filepath);
//     }
// };

// TEST_F(RegisterTest, ValidationTest) {
//     auto result = cut->username_validation("Patryk");
//     EXPECT_TRUE(result);

//     auto result2 = cut->username_validation("");
//     EXPECT_FALSE(result2);

//     auto user = cut->password_validation("Patryk123!");
//     EXPECT_TRUE(user);

//     auto user2 = cut->password_validation("patryk");
//     EXPECT_FALSE(user2);
// }

// TEST_F(RegisterTest, ChecksSingupOverallFunctionality) {
//     EXPECT_CALL(*ih, get_input())
//         .WillOnce(testing::Return("Patryk"))
//         .WillOnce(testing::Return("Patryk123!"));

//     tdc::User expectedUser{1, "Patryk", tdc::Role::Admin,
//                            tdl::hash("Patryk123!")};

//     auto user_ = cut->singup();
//     EXPECT_TRUE(user_);
//     EXPECT_EQ(user_.unwrap(), expectedUser);
// }

// struct LoginTest : testing::Test {
//     std::shared_ptr<tdc::UserDb> udb =
//         std::make_shared<tdc::UserDb>(TEST_DB_PATH);
//     std::shared_ptr<MockUserInputHandler> ih =
//         std::make_shared<MockUserInputHandler>();
//     std::shared_ptr<MockDisplayer> d = std::make_shared<MockDisplayer>();
//     std::unique_ptr<td::AuthManager> am =
//         std::make_unique<td::AuthManager>(udb, ih, d);
//     String username = "SomeUser";
//     String password = "VeryStrongPassword123!";
//     tdc::User added_user =
//         tdc::User{username, tdc::Role::User, tdl::hash(password)};

//     void SetUp() override {
//         udb->add_object(added_user);
//         auto user = udb->get_object_by_id(added_user.id());
//         added_user.set_id(id.unwrap());
//     }

//     void TearDown() override {
//         am.reset();
//         udb.reset();
//         ih.reset();
//         d.reset();
//         fs::path filepath = fs::path(TEST_DB_PATH) / String(DB_FILE_EXT);
//     fs:
//         remove(filepath);
//     }
// };

// TEST_F(LoginTest, LoginCorrectness) {
//     EXPECT_CALL(*ih, get_input())
//         .WillOnce(testing::Return(username))
//         .WillOnce(testing::Return(password));

//     auto user = am->login();
//     EXPECT_TRUE(user);
//     EXPECT_EQ(user.unwrap(), added_user);
// }

// TEST_F(LoginTest, AuthCorrectness) {}

// TEST(TaskDbTest, CheckTaskDbOverallCorrectness) {
//     std::unique_ptr<tdc::TaskDb> tdb =
//         std::make_unique<tdc::TaskDb>(TEST_DB_PATH);
//     auto task = tdc::Task{
//         "sometopic", "somecontent", tdl::give_date(0), tdl::give_date(5), 1,
//         2, 0,           false};

//     EXPECT_TRUE(tdb->add_task(task));
//     auto db_task = tdb->get_task(task.id());
//     EXPECT_TRUE(db_task);
//     EXPECT_EQ(db_task.unwrap(), task);
//     tdb.reset();
//     fs::path filepath = fs::path(TEST_DB_PATH);
// fs:
//     remove(filepath);
// }