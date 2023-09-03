cmake_minimum_required(VERSION 3.5)
project(twodo
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_subdirectory(src/lib)
add_subdirectory(src/main)
add_subdirectory(test)

