option(BUILD_GLFW "Build the glfw display platform" ON)
option(BUILD_GLFW_OPENGL "Build the opengl support of the glfw display platform" ON) # Depends on BUILD_OPENGL
option(BUILD_GLFW_VULKAN "Build the vulkan support of the glfw display platform" ON) # Depends on BUILD_VULKAN
option(BUILD_OPENGL "Build the OpenGL gpu platform" ON)
option(BUILD_VULKAN "Build the Vulkan gpu platform" ON)
option(BUILD_BOX2D "Build the box2d physics platform" ON)
option(BUILD_BULLET3 "Build the bullet3 physics platform"  ON)
option(BUILD_OPENAL "Build the OpenAL audio platform"  ON)
option(BUILD_FREETYPE "Build the FreeType font rendering platform"  ON)
option(BUILD_ASSIMP "Build the AssImp resource parser platform (For 3D asset file formats)"  ON)
option(BUILD_SNDFILE "Build the SndFile resource parser platform (For Audio file formats)"  ON)
option(BUILD_GLSLANG "Build the GLSLang shader compiler platform"  ON)
option(BUILD_SPIRVCROSS "Build the SPIRV-Cross shader decompiler platform"  ON)
option(BUILD_CRYPTOPP "Build the CryptoPP platform"  ON)
option(BUILD_ANDROID "Build the android display platform" OFF)
option(BUILD_ANDROID_OPENGL "Build the opengl support of the android display platform" OFF) # Depends on BUILD_OPENGL

option(BUILD_OPENGL2 "Build the OpenGL2 gpu platform" ON)

set(PLATFORM_INCLUDE) # The platform include directories in a list
set(PLATFORM_SRC) # The platform source files in a list
set(PLATFORM_LINK) # The platform linked library names in a list
set(PLATFORM_CLASSES) # The platform class names
set(PLATFORM_INCLUDES) # The platform include paths
set(PLATFORM_NAMES) # The platform names

set(Platform.GLOBEXPR) # The globexpr used to generate PLATFORM_SRC

# @COMPILE_DEFS = Compile definitions
# @DIR = The directory in platform/ which contains the platform source
# @CLASS = The class name that this platform defines
# @BUILD_LINK = The library name/s which the platform links to. There can be multiple BUILD_LINK arguments.
function(AddPlatform COMPILE_DEFS DIR CLASS)
    set(Platform.GLOBEXPR ${Platform.GLOBEXPR} ${BASE_SOURCE_DIR}/platform/${DIR}/*.cpp ${BASE_SOURCE_DIR}/platform/${DIR}/*.c PARENT_SCOPE)
    add_compile_definitions(${COMPILE_DEFS})
    set(PLATFORM_INCLUDE ${PLATFORM_INCLUDE} ${BASE_SOURCE_DIR}/platform/${DIR}/ PARENT_SCOPE)
    set(PLATFORM_CLASSES "${PLATFORM_CLASSES}${CLASS};" PARENT_SCOPE)
    set(PLATFORM_NAMES "${PLATFORM_NAMES}${DIR};" PARENT_SCOPE)
    if (${ARGC} GREATER 3)
        # Each additional argument is treated as a library name
        set(MAXINDEX ${ARGC})
        MATH(EXPR MAXINDEX "${MAXINDEX}-1")
        foreach (index RANGE 3 ${MAXINDEX})
            list(GET ARGV ${index} LIBNAME)
            set(BUILD_LINK ${BUILD_LINK} ${LIBNAME})
        endforeach ()
        set(PLATFORM_LINK ${PLATFORM_LINK} ${BUILD_LINK} PARENT_SCOPE)
    endif ()
endfunction()

### --  Start platform Definitions -- ###

if (BUILD_GLFW)
    AddPlatform(BUILD_GLFW
            glfw
            glfw::GLFWDisplayDriver
            glfw)
endif ()

if (BUILD_GLFW_OPENGL)
    add_compile_definitions(BUILD_GLFW_OPENGL)
endif ()

if (BUILD_GLFW_VULKAN)
    add_compile_definitions(BUILD_GLFW_VULKAN)
endif ()

if (BUILD_ANDROID)
    AddPlatform(BUILD_ANDROID
            android
            android::AndroidDisplayDriver
            android
            EGL)
endif ()

if (BUILD_ANDROID_OPENGL)
    add_compile_definitions(BUILD_ANDROID_OPENGL)
endif ()

if (BUILD_OPENGL)
    if (ANDROID)
        set(GL_LIBNAME GLESv3)
    elseif (WIN32)
        set(GL_LIBNAME OpenGL32)
    else ()
        set(GL_LIBNAME GL)
    endif ()
    AddPlatform(BUILD_OPENGL
            opengl
            opengl::OGLGpuDriver
            ${GL_LIBNAME})
endif ()

if (BUILD_OPENGL2)
    if (ANDROID)
        set(GL_LIBNAME GLESv3)
    elseif (WIN32)
        set(GL_LIBNAME OpenGL32)
    else ()
        set(GL_LIBNAME GL)
    endif ()
    AddPlatform(BUILD_OPENGL2
            opengl2
            OpenGL2
            ${GL_LIBNAME})
endif ()

if (BUILD_VULKAN)
    find_package(Vulkan REQUIRED)
    include_directories(${Vulkan_INCLUDE_DIRS})
    AddPlatform(BUILD_VULKAN
            vulkan
            vulkan::VkGpuDriver
            ${Vulkan_LIBRARIES})
endif ()

if (BUILD_BOX2D)
    AddPlatform(BUILD_BOX2D
            box2d
            PhysicsDriverBox2D
            box2d)
endif ()

if (BUILD_BULLET3)
    AddPlatform(BUILD_BULLET3
            bullet3
            PhysicsDriverBt3
            BulletDynamics BulletCollision LinearMath)
endif ()

if (BUILD_OPENAL)
    AddPlatform(BUILD_OPENAL
            openal-soft
            OALAudioDriver
            OpenAL)
endif ()

if (BUILD_FREETYPE)
    AddPlatform(BUILD_FREETYPE
            freetype
            FtFontDriver
            freetype)
endif ()

if (BUILD_ASSIMP)
    AddPlatform(BUILD_ASSIMP
            assimp
            AssImpImporter
            assimp)
endif ()

if (BUILD_SNDFILE)
    AddPlatform(BUILD_SNDFILE
            sndfile
            SndFileImporter
            sndfile)
endif ()

if (BUILD_GLSLANG)
    AddPlatform(BUILD_GLSLANG
            glslang
            GlslangCompiler
            glslang SPIRV glslang-default-resource-limits MachineIndependent OSDependent GenericCodeGen OGLCompiler)
endif ()

if (BUILD_SPIRVCROSS)
    AddPlatform(BUILD_SPIRVCROSS
            spirv-cross
            SpirvCrossDecompiler
            spirv-cross-core spirv-cross-glsl spirv-cross-hlsl)
endif ()

if (BUILD_CRYPTOPP)
    AddPlatform(BUILD_CRYPTOPP
            cryptopp
            CryptoPPDriver
            cryptopp)
endif ()

### --  Stop platform Definitions -- ###

set(STR_LIBRARIES "")
foreach (val IN LISTS PLATFORM_LINK)
    set(STR_LIBRARIES "${STR_LIBRARIES}${val} ")
endforeach ()

list(LENGTH PLATFORM_INCLUDE LEN_PLATFORM_INCLUDE)

message("${LEN_PLATFORM_INCLUDE} Platforms will be built")
foreach (val IN LISTS PLATFORM_INCLUDE)
    message("   ${val}")
endforeach ()

if (${LEN_PLATFORM_INCLUDE} GREATER 0)
    message("Linked Libraries: ${STR_LIBRARIES}")
endif ()

file(GLOB_RECURSE PLATFORM_SRC ${Platform.GLOBEXPR})
