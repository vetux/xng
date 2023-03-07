cmake_minimum_required(VERSION 3.0.0)

project(xEngine-Android-x86)

include(user-config-project.cmake OPTIONAL)

include(${BASE_SOURCE_DIR}/cmake/setup.cmake)
include(${BASE_SOURCE_DIR}/cmake/drivers.cmake)
include(${BASE_SOURCE_DIR}/cmake/engine.cmake)

message("Building android-x86 ${CMAKE_BUILD_TYPE}")