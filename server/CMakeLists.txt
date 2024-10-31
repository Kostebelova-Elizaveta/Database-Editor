cmake_minimum_required(VERSION 3.15.0)
cmake_policy(SET CMP0016 NEW)

project(http_server VERSION 1.0.0 LANGUAGES CXX DESCRIPTION "HTTP-сервер")

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Sql)
find_package(GTest REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/server/src/http_server)
include_directories(${CMAKE_SOURCE_DIR}/server/src/http_server/include)
include_directories(${CMAKE_SOURCE_DIR}/server/src/http_server/config)
include_directories(${GTEST_INCLUDE_DIRS})

qt5_wrap_cpp(moc_httpserver.cpp ${CMAKE_SOURCE_DIR}/server/src/http_server/include/httpserver.h)

set(SERVER_CONFIG_FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/http_server/config/config.ini")
add_definitions(-DSERVER_CONFIG_FILE_PATH="${SERVER_CONFIG_FILE_PATH}")

set(PROJECT_SOURCES
        ./src/http_server/main.cpp
        ./src/http_server/httpserver.cpp ./src/http_server/include/httpserver.h
        ./src/http_server/logger.cpp ./src/http_server/include/logger.h)

add_executable(http_server ${PROJECT_SOURCES} ${SERVER_CONFIG_FILE_PATH} ${moc_httpserver.cpp})

target_link_libraries(http_server Qt5::Core Qt5::Network Qt5::Sql)

# Tests
qt5_wrap_cpp(moc_httpserver_test.cpp ${CMAKE_SOURCE_DIR}/server/src/http_server/include/httpserver.h)

add_executable(TestServer
    ${CMAKE_SOURCE_DIR}/server/test/testServer.cpp
    ${CMAKE_SOURCE_DIR}/server/src/http_server/httpserver.cpp
    ${CMAKE_SOURCE_DIR}/server/src/http_server/include/logger.h
    ${CMAKE_SOURCE_DIR}/server/src/http_server/logger.cpp
    ${moc_httpserver_test.cpp}
)

target_link_libraries(TestServer GTest::GTest GTest::Main Qt5::Core Qt5::Network Qt5::Sql)

add_test(NAME TestServer COMMAND TestServer)
