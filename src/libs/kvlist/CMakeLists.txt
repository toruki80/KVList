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
    src/kvlist_global.h
    src/kvlist_global.cpp

    include/kvlistentry.h
    src/kvlistentry.cpp

    include/kvlistmodel.h
    src/kvlistmodel.cpp

    include/kvlistfilteredmodel.h
    src/kvlistfilteredmodel.cpp


    src/kvlistserializer.h
    src/kvlistserializer.cpp

    src/kvlistserializerxml.h
    src/kvlistserializerxml.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Xml)
target_compile_definitions(${PROJECT_NAME} PRIVATE KVLIST_LIBRARY)
target_include_directories(${PROJECT_NAME} PUBLIC include src)

install(TARGETS ${PROJECT_NAME}
    COMPONENT application
    DESTINATION .
)
