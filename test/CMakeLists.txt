set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

find_package(GTest REQUIRED)
include(GoogleTest)

add_subdirectory(../thirdparty/googletest build)

file(GLOB TEST_SRC *.cpp)

add_executable(${PROJECT_NAME}_ut ${TEST_SRC})

target_link_libraries(
    ${PROJECT_NAME}_ut
    ${PROJECT_NAME}_lib
    GTest::gtest
    GTest::gtest_main
)

gtest_discover_tests(${PROJECT_NAME}_ut WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_test(NAME ${PROJECT_NAME}_tst COMMAND ${PROJECT_NAME}_ut)
