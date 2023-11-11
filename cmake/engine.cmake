set(Engine.Dir.INCLUDE ${BASE_SOURCE_DIR}/engine/include/)
set(Engine.Dir.SRC ${BASE_SOURCE_DIR}/engine/src/)

file(GLOB_RECURSE Engine.File.SRC ${Engine.Dir.SRC}*.cpp ${Engine.Dir.SRC}*.c)

# xengine

add_library(xengine SHARED ${Engine.File.SRC} ${DRIVERS_SRC} ${SHADER_HEADERS})

target_include_directories(xengine PUBLIC ${Engine.Dir.INCLUDE})
target_include_directories(xengine PRIVATE ${Engine.Dir.SRC} ${DRIVERS_INCLUDE} ${SHADER_COMPILED_DIR})

target_link_libraries(xengine Threads::Threads ${DRIVERS_LINK})

if (UNIX AND CMAKE_COMPILER_IS_GNUCXX)
    # Hide symbols by default on GCC to emulate msvc linking behaviour and enable Pedantic warnings to emulate msvc syntax checking.
    target_compile_options(xengine PUBLIC -fvisibility=hidden -pedantic)
endif ()

if (UNIX)
    target_link_libraries(xengine dl) # For engine/src/io/dl/librarylinux.hpp
endif ()

if (MSVC)
# Disable the C4251 warnings because the STL is heavily integrated with the engine by design and the users/editor must ensure that the user application is built with the same msvc compiler version.
	target_compile_options(xengine PUBLIC -wd4251)
endif ()

# xengine-static

add_library(xengine-static STATIC ${Engine.File.SRC} ${DRIVERS_SRC} ${SHADER_HEADERS})

target_include_directories(xengine-static PUBLIC ${Engine.Dir.INCLUDE})
target_include_directories(xengine-static PRIVATE ${Engine.Dir.SRC} ${DRIVERS_INCLUDE} ${SHADER_COMPILED_DIR})

target_link_libraries(xengine-static Threads::Threads ${DRIVERS_LINK})

if (UNIX AND CMAKE_COMPILER_IS_GNUCXX)
    # Hide symbols by default on GCC to emulate msvc linking behaviour and enable Pedantic warnings to emulate msvc syntax checking.
    target_compile_options(xengine-static PUBLIC -fvisibility=hidden -pedantic)
endif ()

if (UNIX)
    target_link_libraries(xengine-static dl)
endif ()

if (MSVC)
# Disable the C4251 warnings because the STL is heavily integrated with the engine by design and the users/editor must ensure that the user application is built with the same msvc compiler version.
	target_compile_options(xengine-static PUBLIC -wd4251)
endif ()
