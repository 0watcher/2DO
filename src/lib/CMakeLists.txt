set(SQLITECPP_RUN_CPPCHECK OFF CACHE BOOL "" FORCE)
set(SQLITECPP_RUN_CPPLINT OFF CACHE BOOL "" FORCE)

set(SQLITECPP_DIR "${CMAKE_SOURCE_DIR}/thirdparty/SQLiteCpp")
add_subdirectory(${SQLITECPP_DIR} ${CMAKE_BINARY_DIR}/SQLiteCpp)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

add_library(${PROJECT_NAME}_lib STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${SQLITECPP_DIR}/include
)

target_link_libraries(
    ${PROJECT_NAME}_lib
    SQLiteCpp
)