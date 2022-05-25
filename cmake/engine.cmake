set(Engine.Dir.INCLUDE engine/include/)
set(Engine.Dir.SRC engine/src/)

file(GLOB_RECURSE Engine.File.SRC ${Engine.Dir.SRC}*.cpp ${Engine.Dir.SRC}*.c)

add_library(xengine SHARED ${Engine.File.SRC} ${DRIVERS_SRC})

target_include_directories(xengine PUBLIC ${Engine.Dir.INCLUDE})
target_include_directories(xengine PRIVATE ${Engine.Dir.SRC} ${DRIVERS_INCLUDE})

target_link_libraries(xengine
        Threads::Threads
        freetype
        shaderc_combined
        spirv-cross-core
        spirv-cross-glsl
        spirv-cross-hlsl
        imgui
        assimp
        sndfile
        cryptopp
        ${DRIVERS_LINK})

if (UNIX AND CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(xengine PUBLIC -fvisibility=hidden)
endif ()