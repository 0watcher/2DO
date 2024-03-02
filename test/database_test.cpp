#include <2DOApp/term.hpp>
#include <2DOCore/task.hpp>
#include <2DOCore/user.hpp>
#include <Utils/result.hpp>
#include <Utils/type.hpp>
#include <Utils/util.hpp>

#include <gtest/gtest.h>
#include <memory>
#include <optional>

namespace tdc = twodocore;
namespace tdu = twodoutils;

constexpr StringView TEST_DB_PATH = ":memory:";

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
    }
};

TEST_F(DbTest, CheckUserDbFunctionalities) {
    tdc::User user{"patryk", tdc::Role::Admin, "Patryk123!"};
    EXPECT_NO_THROW(user_db->add_object(user));

    std::optional<tdc::User> user_;
    EXPECT_NO_THROW(user_ = user_db->get_object(user.id()));
    EXPECT_EQ(user_, user);

    user.set_password("$SuperSecretPass2");
    user.set_username("SomeNewUser");
    EXPECT_NO_THROW(user_db->update_object(user));

    std::optional<tdc::User> user__;
    EXPECT_NO_THROW(user__ = user_db->get_object(user.id()));
    EXPECT_EQ(user__, user);

    tdc::User user2{"someguy", tdc::Role::User, "Password123!"};
    EXPECT_NO_THROW(user_db->add_object(user2));
    Vector<tdc::User> users;
    EXPECT_NO_THROW(users = user_db->get_all_objects());
    EXPECT_EQ(users[0], user);
    EXPECT_EQ(users[1], user2);

    std::optional<tdc::User> user___;
    EXPECT_NO_THROW(user___ =
                        user_db->find_object_by_unique_column(user.username()));
    EXPECT_EQ(user___, user);

    EXPECT_NO_THROW(user_db->delete_object(user.id()));
    EXPECT_NO_THROW(user_db->delete_object(user2.id()));
    EXPECT_TRUE(user_db->is_table_empty());
}

TEST_F(DbTest, CheckTaskDbFunctionalities) {
    tdc::Task task{"SomeTopic",
                   "There is so much to do!",
                   tdu::get_current_timestamp<TimePoint>(),
                   tdu::get_current_timestamp<TimePoint>(5),
                   1,
                   2,
                   false};
    EXPECT_NO_THROW(task_db->add_object(task));

    std::optional<tdc::Task> task_;
    EXPECT_TRUE(task_ = task_db->get_object(task.id()));
    EXPECT_EQ(task_, task);

    task.set_topic("SomeOtherTopic");
    task.set_content("I've added some additional things to do now!");
    task.set_deadline(tdu::get_current_timestamp<TimePoint>());
    EXPECT_NO_THROW(task_db->update_object(task));

    std::optional<tdc::Task> task__;
    EXPECT_NO_THROW(task__ = task_db->get_object(task.id()));
    EXPECT_EQ(task__, task);

    tdc::Task task2{"SomeOtherTopic",
                    "........",
                    tdu::get_current_timestamp<TimePoint>(),
                    tdu::get_current_timestamp<TimePoint>(10),
                    1,
                    2,
                    false};
    EXPECT_NO_THROW(task_db->add_object(task2));
    Vector<tdc::Task> tasks;
    EXPECT_NO_THROW(
        tasks = task_db->get_all_objects<tdc::TaskDb::IdType::Executor>(1));
    EXPECT_EQ(tasks[0], task);
    EXPECT_EQ(tasks[1], task2);

    EXPECT_NO_THROW(task_db->delete_object(task.id()));
    EXPECT_NO_THROW(task_db->delete_object(task2.id()));
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
        EXPECT_NO_THROW(msg_db->add_object(msg));
    }

    Vector<tdc::Message> selected_messages;
    EXPECT_NO_THROW(selected_messages = msg_db->get_all_objects(1));

    for (std::size_t i = 0; i < messages.size(); ++i) {
        EXPECT_EQ(messages[i], selected_messages[i]);
    }

    EXPECT_NO_THROW(msg_db->delete_all_by_task_id(1));
    EXPECT_TRUE(msg_db->is_table_empty());
}
