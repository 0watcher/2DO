cmake_minimum_required(VERSION 3.5)

project(2DOCore)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(${PROJECT_NAME} ${SOURCES})
add_library(2DOCore::2DOCore ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/SQLiteCpp SQLiteCpp)
target_link_libraries(2DOCore
  SQLiteCpp
)

add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/fmt fmt)
target_link_libraries(2DOCore
  fmt
)