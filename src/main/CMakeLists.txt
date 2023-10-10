set(CMAKE_BUILD_TYPE Debug)

add_executable(main main.cpp)

target_compile_options(main PRIVATE -Wall -Wextra -pedantic)

target_link_libraries(main ${PROJECT_NAME}_lib)