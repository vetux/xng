cmake_minimum_required(VERSION 3.0.0)

project(xEngine)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS_RELEASE -O3)
endif ()

include(cmake/imgui.cmake)
include(cmake/config.cmake)

if (UNIX)
    add_compile_definitions(XENGINE_EXPORT=__attribute__\(\(visibility\("default"\)\)\))
elseif (WIN32)
    add_compile_definitions(XENGINE_EXPORT=__declspec\(dllexport\))
else ()
    add_compile_definitions(XENGINE_EXPORT=)
endif ()

include(cmake/xengine.cmake)