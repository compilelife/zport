cmake_minimum_required(VERSION 2.8)

project(zport)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall")

aux_source_directory(src SRC_LIST)
include_directories(src)

list(APPEND SRC_LIST ${READER}/reader.cpp)
include_directories(${READER})

if (BUILD_TEST)
    set(BUILD_EXE 0)
endif()

if (BUILD_EXE)
    set(BUILD_TEST 0)
    list(APPEND SRC_LIST main.cpp)
    add_executable(${PROJECT_NAME} ${SRC_LIST})
else()
    add_library(${PROJECT_NAME} SHARED ${SRC_LIST})
endif()

if (BUILD_TEST)
    add_subdirectory(test)
endif()