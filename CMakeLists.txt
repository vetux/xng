cmake_minimum_required(VERSION 3.0.0)

if (UNIX)
    # When the linked spirv-cross binaries are built using gcc-11 it results in a segfault when compiling spirv for now the linux build will use gcc-10 by default
    # and the linked SPIRV-Cross binaries must be built using gcc-10.
    # Users can override the compiler version in the user-config.cmake file
    message(WARNING "SPIRV-Cross is not compatible with gcc-11. Using gcc-10")
    set(CMAKE_C_COMPILER "gcc-10")
    set(CMAKE_CXX_COMPILER "g++-10")
endif()

include(user-config.cmake OPTIONAL)

project(xEngine)

include(user-config-project.cmake OPTIONAL)

set(BASE_SOURCE_DIR ${CMAKE_SOURCE_DIR})

include(${BASE_SOURCE_DIR}/cmake/setup.cmake)

include(${BASE_SOURCE_DIR}/cmake/shadercompiler.cmake)
include(${BASE_SOURCE_DIR}/cmake/compileshaders.cmake)

include(${BASE_SOURCE_DIR}/cmake/drivers.cmake)
include(${BASE_SOURCE_DIR}/cmake/engine.cmake)
include(${BASE_SOURCE_DIR}/cmake/tests.cmake)

include(platforms/android/android.cmake)
include(platforms/mingw/mingw.cmake)