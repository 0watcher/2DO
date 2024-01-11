#include <Utils/result.hpp>

#include <gtest/gtest.h>

using namespace twodoutils;

TEST(ResultTest, OkWithValue) {
    auto result = Ok<int, None>(42);
    ASSERT_TRUE(result);
    EXPECT_EQ(result.value(), 42);
}

TEST(ResultTest, OkWithConstValue) {
    const int value = 42;
    const auto result = Ok<int, None>(value);
    ASSERT_TRUE(result);
    EXPECT_EQ(result.value(), value);
}

TEST(ResultTest, ErrWithError) {
    auto result = Err<int, std::string>("Error message");
    ASSERT_FALSE(result);
    EXPECT_EQ(result.err(), "Error message");
}

TEST(ResultTest, Accessors) {
    auto result = Ok<int, std::string>(42);
    ASSERT_TRUE(result);
    EXPECT_EQ(result.value(), 42);

    result = Err<int, std::string>("Error message");
    ASSERT_FALSE(result);
    EXPECT_EQ(result.err(), "Error message");
}
