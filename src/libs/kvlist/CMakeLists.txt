cmake_minimum_required(VERSION 3.10)

project(kvlist LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Xml REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Xml REQUIRED)

add_library(kvlist SHARED
    kvlist_global.h
    kvlist_global.cpp

    kvlistentry.h
    kvlistentry.cpp

    kvlistmodel.h
    kvlistmodel.cpp

    kvlistfilteredmodel.h
    kvlistfilteredmodel.cpp


    kvlistserializer.h
    kvlistserializer.cpp

    kvlistserializerxml.h
    kvlistserializerxml.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Xml)
target_compile_definitions(${PROJECT_NAME} PRIVATE KVLIST_LIBRARY)
target_include_directories(${PROJECT_NAME} PUBLIC .)

install(TARGETS ${PROJECT_NAME}
    COMPONENT application
    DESTINATION .
)
