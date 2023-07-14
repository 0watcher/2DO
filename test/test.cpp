#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "result.hpp"

using namespace twodo;

struct ResultTest : public testing::Test
{
  auto some_func(const int& value) -> Result<int>
  {
    if(value == 0)
    {
      return Result<int>(ErrorCode::overall_err);
    }
    return Result<int>(1);
  }
};

TEST_F(ResultTest, ShouldReturnReturnValue) {
    auto result = some_func(1);

    if (result.ok()) {
        int value = result.unwrap();
        EXPECT_EQ(value, 1);
}
}

TEST_F(ResultTest, ShouldReturnErrorValue) {
    auto result = some_func(0);

    if (result.err()) 
    {
        auto error = result.unwrap_err();
        EXPECT_EQ(error, ErrorCode::overall_err); 
    }
}

#include "user.hpp"

struct UserTest : public testing::Test
{
  Login l{};
};

TEST_F(UserTest, ShouldReturnUserData)
{
  auto nickname = l.nickname();
  auto password = l.password();

}
