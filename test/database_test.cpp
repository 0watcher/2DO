#include <2DOApp/term.hpp>
#include <2DOCore/task.hpp>
#include <2DOCore/user.hpp>
#include <Utils/database.hpp>
#include <Utils/result.hpp>
#include <Utils/type.hpp>
#include <Utils/util.hpp>

#include <gtest/gtest.h>
#include <filesystem>
#include <memory>

namespace tdc = twodocore;
namespace tdu = twodoutils;
namespace fs = std::filesystem;

constexpr StringView TEST_DB_PATH = "../../test/db.db3";

struct DbTest : testing::Test {
    std::unique_ptr<tdc::UserDb> user_db =
        std::make_unique<tdc::UserDb>(TEST_DB_PATH);
    std::unique_ptr<tdc::TaskDb> task_db =
        std::make_unique<tdc::TaskDb>(TEST_DB_PATH);
    std::unique_ptr<tdc::MessageDb> msg_db =
        std::make_unique<tdc::MessageDb>(TEST_DB_PATH);

    void TearDown() override {
        user_db.reset();
        task_db.reset();
        fs::path filepath = fs::path(TEST_DB_PATH);
        fs::remove(filepath);
    }
};

TEST_F(DbTest, CheckUserDbFunctionalities) {
    tdc::User user{"patryk", tdc::Role::Admin, tdu::hash("Patryk123!")};
    EXPECT_TRUE(user_db->add_object(user));

    const auto user_ = user_db->get_object_by_id(user.id());
    EXPECT_TRUE(user_);
    EXPECT_EQ(user_.unwrap(), user);

    user.set_password(tdu::hash("$SuperSecretPass2"));
    user.set_username("SomeNewUser");
    EXPECT_TRUE(user_db->update_object(user));

    const auto user__ = user_db->get_object_by_id(user.id());
    EXPECT_TRUE(user__);
    EXPECT_EQ(user__.unwrap(), user);

    tdc::User user2{"someguy", tdc::Role::User, tdu::hash("Password123!")};
    EXPECT_TRUE(user_db->add_object(user2));
    const auto result = user_db->get_all_objects();
    EXPECT_TRUE(result);
    const auto users = result.unwrap();
    EXPECT_EQ(users[0], user);
    EXPECT_EQ(users[1], user2);

    EXPECT_TRUE(user_db->delete_object(user));
    EXPECT_TRUE(user_db->delete_object(user2));
    EXPECT_TRUE(user_db->is_table_empty());
}

TEST_F(DbTest, CheckTaskDbFunctionalities) {
    tdc::User owner{"patryk", tdc::Role::Admin, tdu::hash("Patryk123!")};
    tdc::User executor{"someguy", tdc::Role::User, tdu::hash("Password123!")};
    EXPECT_TRUE(user_db->add_object(owner));
    EXPECT_TRUE(user_db->add_object(executor));

    tdc::Task task{"SomeTopic",
                   "There is so much to do!",
                   tdu::get_current_timestamp<TimePoint>(),
                   tdu::get_current_timestamp<TimePoint>(5),
                   executor.id(),
                   owner.id(),
                   false};
    EXPECT_TRUE(task_db->add_object(task));

    const auto task_ = task_db->get_object_by_id(task.id());
    EXPECT_TRUE(task_);
    EXPECT_EQ(task_.unwrap(), task);

    task.set_topic("SomeOtherTopic");
    task.set_content("I've added some additional things to do now!");
    task.set_deadline(tdu::get_current_timestamp<TimePoint>());
    EXPECT_TRUE(task_db->update_object(task));

    const auto task__ = task_db->get_object_by_id(task.id());
    EXPECT_TRUE(task__);
    EXPECT_EQ(task__.unwrap(), task);

    tdc::Task task2{"SomeOtherTopic",
                    "........",
                    tdu::get_current_timestamp<TimePoint>(),
                    tdu::get_current_timestamp<TimePoint>(10),
                    executor.id(),
                    owner.id(),
                    false};
    EXPECT_TRUE(task_db->add_object(task2));
    const auto result = task_db->get_all_objects();
    EXPECT_TRUE(result);
    const auto tasks = result.unwrap();
    EXPECT_EQ(tasks[0], task);
    EXPECT_EQ(tasks[1], task2);

    EXPECT_TRUE(task_db->delete_object(task));
    EXPECT_TRUE(task_db->delete_object(task2));
    EXPECT_TRUE(task_db->is_table_empty());
}

TEST_F(DbTest, CheckMessageDbFunctionalities) {
    Array<tdc::Message, 3> messages = {
        tdc::Message{1, "someguy", "Hello!",
                     tdu::get_current_timestamp<TimePoint>()},
        tdc::Message{1, "someotherguy", "Hi!",
                     tdu::get_current_timestamp<TimePoint>()},
        tdc::Message{1, "someguy", "Who asked!",
                     tdu::get_current_timestamp<TimePoint>()}};

    for (auto& msg : messages) {
        EXPECT_TRUE(msg_db->add_object(msg));
    }

    auto result = msg_db->get_all_objects();
    EXPECT_TRUE(result);
    const auto selected_messages = result.unwrap();

    for (std::size_t i = 0; i < messages.size(); ++i) {
        EXPECT_EQ(messages[i], selected_messages[i]);
    }

    EXPECT_TRUE(msg_db->delete_all_by_task_id(1));
    EXPECT_TRUE(msg_db->is_table_empty());
}
