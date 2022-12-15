cmake_minimum_required(VERSION 3.0.0)

project(xEngine)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

set(RELEASE_COMPILER_FLAGS)
set(DEBUG_COMPILER_FLAGS)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(RELEASE_COMPILER_FLAGS -O3)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # Optional sanitization flags, breaks debugging
        # set(DEBUG_COMPILER_FLAGS "${DEBUG_COMPILER_FLAGS} -fsanitize=leak -fsanitize=undefined -fsanitize-address-use-after-scope")
    endif ()
endif ()

set(CMAKE_C_FLAGS_RELEASE ${RELEASE_COMPILER_FLAGS})
set(CMAKE_CXX_FLAGS_RELEASE ${RELEASE_COMPILER_FLAGS})
set(CMAKE_C_FLAGS_DEBUG ${DEBUG_COMPILER_FLAGS})
set(CMAKE_CXX_FLAGS_DEBUG  ${DEBUG_COMPILER_FLAGS})

include(config.cmake OPTIONAL)

if (UNIX)
    add_compile_definitions(XENGINE_EXPORT=__attribute__\(\(visibility\("default"\)\)\))
elseif (WIN32)
    add_compile_definitions(XENGINE_EXPORT=__declspec\(dllexport\))
else ()
    add_compile_definitions(XENGINE_EXPORT=)
endif ()

include(cmake/drivers.cmake)
include(cmake/engine.cmake)
include(cmake/tools.cmake)