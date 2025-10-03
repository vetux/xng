cmake_minimum_required(VERSION 3.10.0)

project(xEngine)

set(BASE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include(${BASE_SOURCE_DIR}/cmake/setup.cmake)

get_property(includeDirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
get_property(linkDirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY LINK_DIRECTORIES)
get_property(sysIncludeDirs DIRECTORY "" PROPERTY INCLUDE_DIRECTORIES)
set(includeDirs ${sysIncludeDirs})

message("Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message("Include Dirs: ${includeDirs}")
message("Link Dirs: ${linkDirs}")

include(${BASE_SOURCE_DIR}/cmake/thirdparty.cmake)
include(${BASE_SOURCE_DIR}/cmake/adapters.cmake)
include(${BASE_SOURCE_DIR}/cmake/engine.cmake)

option(BUILD_TESTS "Build the test applications" ON)
if (BUILD_TESTS)
    include(${BASE_SOURCE_DIR}/cmake/tests.cmake)
endif ()