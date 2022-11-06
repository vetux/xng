set(Engine.Dir.INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/engine/include/)
set(Engine.Dir.SRC ${CMAKE_CURRENT_SOURCE_DIR}/engine/src/)

file(GLOB_RECURSE Engine.File.SRC ${Engine.Dir.SRC}*.cpp ${Engine.Dir.SRC}*.c)

add_library(xengine SHARED ${Engine.File.SRC} ${DRIVERS_SRC})

target_include_directories(xengine PUBLIC ${Engine.Dir.INCLUDE})
target_include_directories(xengine PRIVATE ${Engine.Dir.SRC} ${DRIVERS_INCLUDE})

target_link_libraries(xengine Threads::Threads ${DRIVERS_LINK})

if (UNIX AND CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(xengine PUBLIC -fvisibility=hidden -pedantic)
endif ()