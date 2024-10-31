cmake_minimum_required(VERSION 3.15.0)
cmake_policy(SET CMP0016 NEW)

project(http_client VERSION 1.0.0 LANGUAGES CXX DESCRIPTION "GUI-клиент редактирования БД")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

find_package(QT NAMES Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/client/src/http_client)
include_directories(${CMAKE_SOURCE_DIR}/client/src/http_client/include)
include_directories(${CMAKE_SOURCE_DIR}/client/src/http_client/config)

set(CLIENT_CONFIG_FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/http_client/config/config.ini")
add_definitions(-DCLIENT_CONFIG_FILE_PATH="${CLIENT_CONFIG_FILE_PATH}")

set(PROJECT_SOURCES
        ./src/http_client/main.cpp
        ./src/http_client/include/mainwindow.h ./src/http_client/mainwindow.cpp ./src/http_client/mainwindow.ui
        ./src/http_client/include/mynetworkclass.h ./src/http_client/mynetworkclass.cpp
        ./src/http_client/include/dialogtoadd.h ./src/http_client/dialogtoadd.cpp ./src/http_client/dialogtoadd.ui
        ./src/http_client/include/logindialog.h ./src/http_client/logindialog.cpp ./src/http_client/logindialog.ui
        ./src/http_client/include/logger.h ./src/http_client/logger.cpp
        ./src/http_client/helpers.cpp ./src/http_client/include/helpers.h)

add_executable(http_client ${PROJECT_SOURCES})

target_link_libraries(http_client PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(http_client PRIVATE Qt5::Core Qt5::Network)

set_target_properties(http_client PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
