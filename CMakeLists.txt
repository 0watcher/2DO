cmake_minimum_required(VERSION 3.0.0)
project(twodo
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_subdirectory(main)
add_subdirectory(src)
add_subdirectory(test)

