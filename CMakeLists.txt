cmake_minimum_required(VERSION 3.15.0)
cmake_policy(SET CMP0016 NEW)

project(cpp-http-sql-qt VERSION 1.0.0 DESCRIPTION "GUI-клиент редактирования БД и HTTP-сервер")

include_directories(${CMAKE_SOURCE_DIR})

enable_testing()

add_subdirectory(client)
add_subdirectory(server)
add_subdirectory(server/test)
