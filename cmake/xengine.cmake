option(BUILD_ENGINE_DISPLAY_GLFW "Build the glfw display implementation" ON)
option(BUILD_ENGINE_RENDERER_OPENGL "Build the OpenGL render support (Window and Render implementations" ON)
option(BUILD_ENGINE_RENDERER_QT "Build the Qt render support (Window and Render implementations)" ON)
option(BUILD_ENGINE_SCRIPT_MONO "Build the mono script implementation" ON)
option(BUILD_ENGINE_PHYSICS_BOX2D "Build the box2d physics implementation" ON)
option(BUILD_ENGINE_AUDIO_OPENAL "Build the OpenAL audio implementation" ON)

set(Engine.Dir.INCLUDE include/)
set(Engine.Dir.SRC src/)

file(GLOB_RECURSE Engine.File.SRC ${Engine.Dir.SRC}*.cpp ${Engine.Dir.SRC}*.c)

add_library(xengine SHARED ${Engine.File.SRC})

target_include_directories(xengine PUBLIC ${Engine.Dir.INCLUDE})
target_include_directories(xengine PUBLIC ${Engine.Dir.SRC})

target_link_libraries(xengine
        Threads::Threads
        freetype
        shaderc
        spirv-cross-core
        spirv-cross-glsl
        spirv-cross-hlsl
        imgui
        assimp
        sndfile
        cryptopp)

if(UNIX AND CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(xengine PUBLIC -fvisibility=hidden)
endif()

if (BUILD_ENGINE_SCRIPT_MONO)
    add_compile_definitions(BUILD_ENGINE_SCRIPT_MONO)
    target_link_libraries(xengine mono-2.0)
endif ()

if (BUILD_ENGINE_PHYSICS_BOX2D)
    add_compile_definitions(BOX2D_VERSION=${BOX2D_VERSION})
    add_compile_definitions(BUILD_ENGINE_PHYSICS_BOX2D)
    target_link_libraries(xengine ${BOX2D_LIB})
endif ()

if (BUILD_ENGINE_AUDIO_OPENAL)
    add_compile_definitions(BUILD_ENGINE_AUDIO_OPENAL)
    target_link_libraries(xengine openal)
endif ()

if (BUILD_ENGINE_DISPLAY_GLFW)
    add_compile_definitions(BUILD_ENGINE_DISPLAY_GLFW)
    target_link_libraries(xengine glfw)
endif ()

if (BUILD_ENGINE_RENDERER_OPENGL)
    add_compile_definitions(BUILD_ENGINE_RENDERER_OPENGL)
    target_link_libraries(xengine GL)
endif ()

if (BUILD_ENGINE_RENDERER_QT)
    add_compile_definitions(BUILD_ENGINE_RENDERER_QT)
    find_package(Qt5Core REQUIRED)
    find_package(Qt5Widgets REQUIRED)
    target_link_libraries(xengine Qt5::Core Qt5::Widgets)
endif ()