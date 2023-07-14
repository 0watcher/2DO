set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

set(SOURCES
    app.cpp
    app.hpp
    result.hpp
    task.cpp
    task.hpp
    user.cpp
    user.hpp
    database.cpp
    database.hpp
)

add_library(${PROJECT_NAME}_lib STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})