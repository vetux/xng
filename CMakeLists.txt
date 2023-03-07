cmake_minimum_required(VERSION 3.0.0)

project(xEngine)
include(user-config-project.cmake OPTIONAL)

set(BASE_SOURCE_DIR ${CMAKE_SOURCE_DIR})

include(${BASE_SOURCE_DIR}/cmake/setup.cmake)
include(${BASE_SOURCE_DIR}/cmake/drivers.cmake)
include(${BASE_SOURCE_DIR}/cmake/engine.cmake)
include(${BASE_SOURCE_DIR}/cmake/tests.cmake)

include(platforms/android.cmake)