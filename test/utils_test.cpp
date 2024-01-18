#include <Utils/result.hpp>
#include <Utils/type.hpp>

#include <gtest/gtest.h>

namespace tdl = twodoutils;

struct ResultTest : testing::Test {
     tdl::Result<String, int> foo(String str) {
        if(str == "str")
            return tdl::Ok(str);
        else
            return tdl::Err(1);
    }   
};

TEST_F(ResultTest, Movability) {
    String str = "str";

    String moved_ptr = foo(std::move(str)).unwrap();

    ASSERT_EQ(&str, &moved_ptr);
}

