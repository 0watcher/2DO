include(GoogleTest)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(../thirdparty/googletest ${CMAKE_BINARY_DIR}/googletest)

set(TEST_SRC
    result_test.cpp
    database_test.cpp
)
add_executable(${PROJECT_NAME}_ut ${TEST_SRC})

target_link_libraries(${PROJECT_NAME}_ut PRIVATE
    2DOCore
    2DOApp
    GTest::gtest
    GTest::gtest_main
    gmock
)

gtest_discover_tests(${PROJECT_NAME}_ut WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME ${PROJECT_NAME}_tst COMMAND ${PROJECT_NAME}_ut)