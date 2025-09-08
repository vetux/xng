option(BUILD_GLFW "Build the glfw display adapter" ON)
option(BUILD_GLFW_OPENGL "Build the opengl support of the glfw display adapter" ON) # Depends on BUILD_OPENGL
option(BUILD_GLFW_VULKAN "Build the vulkan support of the glfw display adapter" ON) # Depends on BUILD_VULKAN
option(BUILD_OPENGL "Build the OpenGL gpu adapter" ON)
option(BUILD_VULKAN "Build the Vulkan gpu adapter" ON)
option(BUILD_BOX2D "Build the box2d physics adapter" ON)
option(BUILD_BULLET3 "Build the bullet3 physics adapter"  ON)
option(BUILD_OPENAL "Build the OpenAL audio adapter"  ON)
option(BUILD_FREETYPE "Build the FreeType font rendering adapter"  ON)
option(BUILD_ASSIMP "Build the AssImp resource parser adapter (For 3D asset file formats)"  ON)
option(BUILD_SNDFILE "Build the SndFile resource parser adapter (For Audio file formats)"  ON)
option(BUILD_GLSLANG "Build the GLSLang shader compiler adapter"  ON)
option(BUILD_SPIRVCROSS "Build the SPIRV-Cross shader decompiler adapter"  ON)
option(BUILD_CRYPTOPP "Build the CryptoPP adapter"  ON)
option(BUILD_ANDROID "Build the android display adapter" OFF)
option(BUILD_ANDROID_OPENGL "Build the opengl support of the android display adapter" OFF) # Depends on BUILD_OPENGL

set(ADAPTER_INCLUDE) # The adapter include directories in a list
set(ADAPTER_SRC) # The adapter source files in a list
set(ADAPTER_LINK) # The adapter linked library names in a list
set(ADAPTER_CLASSES) # The adapter class names
set(ADAPTER_INCLUDES) # The adapter include paths
set(ADAPTER_NAMES) # The adapter names

set(Adapter.GLOBEXPR) # The globexpr used to generate ADAPTER_SRC

# @COMPILE_DEFS = Compile definitions
# @DIR = The directory in adapters/ which contains the adapter source
# @CLASS = The class name that this adapter defines
# @BUILD_LINK = The library name/s which the adapter links to. There can be multiple BUILD_LINK arguments.
function(AddAdapter COMPILE_DEFS DIR CLASS)
    set(Adapter.GLOBEXPR ${Adapter.GLOBEXPR} ${BASE_SOURCE_DIR}/adapters/${DIR}/*.cpp ${BASE_SOURCE_DIR}/adapters/${DIR}/*.c PARENT_SCOPE)
    add_compile_definitions(${COMPILE_DEFS})
    set(ADAPTER_INCLUDE ${ADAPTER_INCLUDE} ${BASE_SOURCE_DIR}/adapters/${DIR}/ PARENT_SCOPE)
    set(ADAPTER_CLASSES "${ADAPTER_CLASSES}${CLASS};" PARENT_SCOPE)
    set(ADAPTER_NAMES "${ADAPTER_NAMES}${DIR};" PARENT_SCOPE)
    if (${ARGC} GREATER 3)
        # Each additional argument is treated as a library name
        set(MAXINDEX ${ARGC})
        MATH(EXPR MAXINDEX "${MAXINDEX}-1")
        foreach (index RANGE 3 ${MAXINDEX})
            list(GET ARGV ${index} LIBNAME)
            set(BUILD_LINK ${BUILD_LINK} ${LIBNAME})
        endforeach ()
        set(ADAPTER_LINK ${ADAPTER_LINK} ${BUILD_LINK} PARENT_SCOPE)
    endif ()
endfunction()

### --  Start adapter Definitions -- ###

if (BUILD_GLFW)
    AddAdapter(BUILD_GLFW
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
    AddAdapter(BUILD_ANDROID
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
    AddAdapter(BUILD_OPENGL
            opengl
            opengl::OGLGpuDriver
            ${GL_LIBNAME})
endif ()

if (BUILD_VULKAN)
    find_package(Vulkan REQUIRED)
    include_directories(${Vulkan_INCLUDE_DIRS})
    AddAdapter(BUILD_VULKAN
            vulkan
            vulkan::VkGpuDriver
            ${Vulkan_LIBRARIES})
endif ()

if (BUILD_BOX2D)
    AddAdapter(BUILD_BOX2D
            box2d
            PhysicsDriverBox2D
            box2d)
endif ()

if (BUILD_BULLET3)
    AddAdapter(BUILD_BULLET3
            bullet3
            PhysicsDriverBt3
            BulletDynamics BulletCollision LinearMath)
endif ()

if (BUILD_OPENAL)
    AddAdapter(BUILD_OPENAL
            openal-soft
            OALAudioDriver
            OpenAL)
endif ()

if (BUILD_FREETYPE)
    AddAdapter(BUILD_FREETYPE
            freetype
            FtFontDriver
            freetype)
endif ()

if (BUILD_ASSIMP)
    AddAdapter(BUILD_ASSIMP
            assimp
            AssImpImporter
            assimp)
endif ()

if (BUILD_SNDFILE)
    AddAdapter(BUILD_SNDFILE
            sndfile
            SndFileImporter
            sndfile)
endif ()

if (BUILD_GLSLANG)
    AddAdapter(BUILD_GLSLANG
            glslang
            GlslangCompiler
            glslang SPIRV glslang-default-resource-limits MachineIndependent OSDependent GenericCodeGen OGLCompiler)
endif ()

if (BUILD_SPIRVCROSS)
    AddAdapter(BUILD_SPIRVCROSS
            spirv-cross
            SpirvCrossDecompiler
            spirv-cross-core spirv-cross-glsl spirv-cross-hlsl)
endif ()

if (BUILD_CRYPTOPP)
    AddAdapter(BUILD_CRYPTOPP
            cryptopp
            CryptoPPDriver
            cryptopp)
endif ()

### --  Stop adapter Definitions -- ###

set(STR_LIBRARIES "")
foreach (val IN LISTS ADAPTER_LINK)
    set(STR_LIBRARIES "${STR_LIBRARIES}${val} ")
endforeach ()

list(LENGTH ADAPTER_INCLUDE LEN_ADAPTER_INCLUDE)

message("${LEN_ADAPTER_INCLUDE} adapters will be built")
foreach (val IN LISTS ADAPTER_INCLUDE)
    message("   ${val}")
endforeach ()

if (${LEN_ADAPTER_INCLUDE} GREATER 0)
    message("Linked Libraries: ${STR_LIBRARIES}")
endif ()

file(GLOB_RECURSE ADAPTER_SRC ${Adapter.GLOBEXPR})
