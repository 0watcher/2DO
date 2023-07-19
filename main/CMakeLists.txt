set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

add_executable(main main.cpp)

target_link_libraries(main ${PROJECT_NAME}_lib)