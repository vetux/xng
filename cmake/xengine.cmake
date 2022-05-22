option(DRIVER_GLFW "Build the glfw display driver" ON)
option(DRIVER_OPENGL "Build the OpenGL render driver (Window and Render implementations" ON)
option(DRIVER_QT_RENDERER "Build the Qt render driver (Window and Render implementations)" ON)
option(DRIVER_MONO "Build the mono script driver" ON)
option(DRIVER_BOX2D "Build the box2d physics driver" ON)
option(DRIVER_BULLET3 "Build the bullet3 physics driver" ON)
option(DRIVER_OPENAL "Build the OpenAL audio driver" ON)

set(Engine.Dir.INCLUDE engine/include/)
set(Engine.Dir.SRC engine/src/)

set(Engine.String.GLOBEXPR ${Engine.Dir.SRC}*.cpp ${Engine.Dir.SRC}*.c)

if (DRIVER_MONO)
    set(Engine.String.GLOBEXPR ${Engine.String.GLOBEXPR} drivers/mono/*.cpp drivers/mono/*.c)
endif ()

if (DRIVER_BOX2D)
    set(Engine.String.GLOBEXPR ${Engine.String.GLOBEXPR} drivers/box2d/*.cpp drivers/box2d/*.c)
endif ()

if (DRIVER_BULLET3)
    set(Engine.String.GLOBEXPR ${Engine.String.GLOBEXPR} drivers/bullet3/*.cpp drivers/bullet3/*.c)
endif ()

if (DRIVER_OPENAL)
    set(Engine.String.GLOBEXPR ${Engine.String.GLOBEXPR} drivers/openal/*.cpp drivers/openal/*.c)
endif ()

if (DRIVER_GLFW)
    set(Engine.String.GLOBEXPR ${Engine.String.GLOBEXPR} drivers/glfw/*.cpp drivers/glfw/*.c)
endif ()

if (DRIVER_OPENGL)
    set(Engine.String.GLOBEXPR ${Engine.String.GLOBEXPR} drivers/opengl/*.cpp drivers/opengl/*.c)
endif ()

file(GLOB_RECURSE Engine.File.SRC ${Engine.String.GLOBEXPR})

add_library(xengine SHARED ${Engine.File.SRC})

target_include_directories(xengine PUBLIC ${Engine.Dir.INCLUDE})
target_include_directories(xengine PUBLIC ${Engine.Dir.SRC})

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
        cryptopp)

if (UNIX AND CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(xengine PUBLIC -fvisibility=hidden)
endif ()

if (DRIVER_MONO)
    add_compile_definitions(DRIVER_MONO)
    target_link_libraries(xengine mono-2.0)
    target_include_directories(xengine PRIVATE drivers/mono/)
endif ()

if (DRIVER_BOX2D)
    add_compile_definitions(BOX2D_VERSION=${BOX2D_VERSION})
    add_compile_definitions(DRIVER_BOX2D)
    target_link_libraries(xengine ${BOX2D_LIB})
    target_include_directories(xengine PRIVATE drivers/box2d/)
endif ()

if (DRIVER_BULLET3)
    target_include_directories(xengine PRIVATE drivers/bullet3/)
endif ()

if (DRIVER_OPENAL)
    add_compile_definitions(DRIVER_OPENAL)
    target_link_libraries(xengine openal)
    target_include_directories(xengine PRIVATE drivers/openal/)
endif ()

if (DRIVER_GLFW)
    add_compile_definitions(DRIVER_GLFW)
    target_link_libraries(xengine glfw)
    target_include_directories(xengine PRIVATE drivers/glfw/)
endif ()

if (DRIVER_OPENGL)
    add_compile_definitions(DRIVER_OPENGL)
    target_link_libraries(xengine GL)
    target_include_directories(xengine PRIVATE drivers/opengl/)
endif ()

if (DRIVER_QT_RENDERER)
    add_compile_definitions(DRIVER_QT_RENDERER)
    find_package(Qt5Core REQUIRED)
    find_package(Qt5Widgets REQUIRED)
    target_link_libraries(xengine Qt5::Core Qt5::Widgets)
endif ()