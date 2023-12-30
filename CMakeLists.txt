cmake_minimum_required(VERSION 3.0.0)

include(user-config.cmake OPTIONAL)

project(xEngine)

include(user-config-project.cmake OPTIONAL)

set(BASE_SOURCE_DIR ${CMAKE_SOURCE_DIR})

include(${BASE_SOURCE_DIR}/cmake/setup.cmake)

include(${BASE_SOURCE_DIR}/cmake/thirdparty.cmake)

get_property(includeDirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
get_property(linkDirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY LINK_DIRECTORIES)
get_property(sysIncludeDirs DIRECTORY "" PROPERTY INCLUDE_DIRECTORIES)
set(includeDirs ${sysIncludeDirs})

message("Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message("Include Dirs: ${includeDirs}")
message("Link Dirs: ${linkDirs}")

set(SHADER_COMPILED_DIR ${BASE_SOURCE_DIR}/compiled_shaders/)

if (NOT DEFINED CROSS_COMPILING)
    include(${BASE_SOURCE_DIR}/cmake/shadercompiler.cmake)
    include(${BASE_SOURCE_DIR}/cmake/compileshaders.cmake)
endif ()

include(${BASE_SOURCE_DIR}/cmake/drivers.cmake)
include(${BASE_SOURCE_DIR}/cmake/engine.cmake)
include(${BASE_SOURCE_DIR}/cmake/tests.cmake)