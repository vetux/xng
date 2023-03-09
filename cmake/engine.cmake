set(Engine.Dir.INCLUDE ${BASE_SOURCE_DIR}/engine/include/)
set(Engine.Dir.SRC ${BASE_SOURCE_DIR}/engine/src/)

file(GLOB_RECURSE Engine.File.SRC ${Engine.Dir.SRC}*.cpp ${Engine.Dir.SRC}*.c)

# xengine

add_library(xengine SHARED ${Engine.File.SRC} ${DRIVERS_SRC})

target_include_directories(xengine PUBLIC ${Engine.Dir.INCLUDE})
target_include_directories(xengine PRIVATE ${Engine.Dir.SRC} ${DRIVERS_INCLUDE})

target_link_libraries(xengine Threads::Threads ${DRIVERS_LINK})

if (UNIX AND CMAKE_COMPILER_IS_GNUCXX)
    # Hide symbols by default on GCC to emulate msvc linking behaviour and enable Pedantic warnings to emulate msvc syntax checking.
    target_compile_options(xengine PUBLIC -fvisibility=hidden -pedantic)
endif ()

if (UNIX)
    target_link_libraries(xengine dl) # For engine/src/io/dl/librarylinux.hpp
endif ()

# xengine-static

add_library(xengine-static STATIC ${Engine.File.SRC} ${DRIVERS_SRC})

target_include_directories(xengine-static PUBLIC ${Engine.Dir.INCLUDE})
target_include_directories(xengine-static PRIVATE ${Engine.Dir.SRC} ${DRIVERS_INCLUDE})

target_link_libraries(xengine-static Threads::Threads ${DRIVERS_LINK})

if (UNIX AND CMAKE_COMPILER_IS_GNUCXX)
    # Hide symbols by default on GCC to emulate msvc linking behaviour and enable Pedantic warnings to emulate msvc syntax checking.
    target_compile_options(xengine-static PUBLIC -fvisibility=hidden -pedantic)
endif ()

if (UNIX)
    target_link_libraries(xengine-static dl)
endif ()