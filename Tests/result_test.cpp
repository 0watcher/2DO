#include <gtest/gtest.h>

#include <Utils/result.hpp>
#include <Utils/type.hpp>
#include "Utils/util.hpp"

namespace tdl = twodoutils;

struct ResultTest : testing::Test {
    enum class Error { DivisionByZero };

    tdl::Result<int, Error> Result_divide(int numerator, int denominator) {
        if (denominator == 0) {
            return tdl::Err(Error::DivisionByZero);
        } else {
            return tdl::Ok(numerator / denominator);
        }
    }

    int Exception_divide(int numerator, int denominator) {
        if (denominator == 0) {
            throw Error::DivisionByZero;
        } else {
            return numerator / denominator;
        }
    }
};

TEST_F(ResultTest, SpeedTest) {
    const auto result_exec_time = twodoutils::speed_test(
        [this]() { auto result = Result_divide(10, 0); });

    const auto exception_exec_time = twodoutils::speed_test([this]() {
        try {
            auto result = Exception_divide(10, 0);
        } catch (const ResultTest::Error& e) {
        }
    });

    EXPECT_LT(result_exec_time.count(), exception_exec_time.count());
}

TEST_F(ResultTest, ValueValidation) {
    auto result = Result_divide(100, 5);
    EXPECT_TRUE(result);
    auto result2 = Result_divide(100, 0);
    EXPECT_FALSE(result2);
}
