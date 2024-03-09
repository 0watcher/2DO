cmake_minimum_required(VERSION 3.5)

project(2DO)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
enable_testing()

add_subdirectory(2DOApp)
add_subdirectory(2DOCore)
add_subdirectory(Utils)
add_subdirectory(Tests)