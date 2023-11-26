option(DRIVER_GLFW "Build the glfw display driver" ON)
option(DRIVER_GLFW_OPENGL "Build the opengl support of the glfw display driver" ON) # Depends on DRIVER_OPENGL
option(DRIVER_GLFW_VULKAN "Build the vulkan support of the glfw display driver" ON) # Depends on DRIVER_VULKAN
option(DRIVER_OPENGL "Build the OpenGL gpu driver" ON)
option(DRIVER_VULKAN "Build the Vulkan gpu driver" ON)
option(DRIVER_BOX2D "Build the box2d physics driver" ON)
option(DRIVER_BULLET3 "Build the bullet3 physics driver"  ON)
option(DRIVER_OPENAL "Build the OpenAL audio driver"  ON)
option(DRIVER_FREETYPE "Build the FreeType font rendering driver"  ON)
option(DRIVER_ASSIMP "Build the AssImp resource parser driver (For 3D asset file formats)"  ON)
option(DRIVER_SNDFILE "Build the SndFile resource parser driver (For Audio file formats)"  ON)
option(DRIVER_GLSLANG "Build the GLSLang shader compiler driver"  ON)
option(DRIVER_SPIRVCROSS "Build the SPIRV-Cross shader decompiler driver"  ON)
option(DRIVER_CRYPTOPP "Build the CryptoPP driver"  ON)
option(DRIVER_ANDROID "Build the android display driver" OFF)
option(DRIVER_ANDROID_OPENGL "Build the opengl support of the android display driver" OFF) # Depends on DRIVER_OPENGL

set(DRIVERS_INCLUDE) # The drivers include directories in a list
set(DRIVERS_SRC) # The drivers source files in a list
set(DRIVERS_LINK) # The drivers linked library names in a list
set(Drivers.GLOBEXPR) # The globexpr used to generate DRIVERS_SRC
set(DRIVERS_CLASSES) # The driver class names
set(DRIVERS_INCLUDES) # The driver include paths
set(DRIVERS_NAMES) # The driver names

# @COMPILE_DEFS = Compile definitions
# @DIR = The directory in drivers/ which contains the driver source
# @CLASS = The class name that this driver defines
# @DRIVER_LINK = The library name/s which the driver links to. There can be multiple DRIVER_LINK arguments.
function(CompileDriver COMPILE_DEFS DIR CLASS)
    set(Drivers.GLOBEXPR ${Drivers.GLOBEXPR} ${BASE_SOURCE_DIR}/drivers/${DIR}/*.cpp ${BASE_SOURCE_DIR}/drivers/${DIR}/*.c PARENT_SCOPE)
    add_compile_definitions(${COMPILE_DEFS})
    set(DRIVERS_INCLUDE ${DRIVERS_INCLUDE} ${BASE_SOURCE_DIR}/drivers/${DIR}/ PARENT_SCOPE)
    set(DRIVERS_CLASSES "${DRIVERS_CLASSES}${CLASS};" PARENT_SCOPE)
    set(DRIVERS_NAMES "${DRIVERS_NAMES}${DIR};" PARENT_SCOPE)
    if (${ARGC} GREATER 3)
        # Each additional argument is treated as a library name
        set(MAXINDEX ${ARGC})
        MATH(EXPR MAXINDEX "${MAXINDEX}-1")
        foreach (index RANGE 3 ${MAXINDEX})
            list(GET ARGV ${index} LIBNAME)
            set(DRIVER_LINK ${DRIVER_LINK} ${LIBNAME})
        endforeach ()
        set(DRIVERS_LINK ${DRIVERS_LINK} ${DRIVER_LINK} PARENT_SCOPE)
    endif ()
endfunction()

### --  Start Driver Definitions -- ###

if (DRIVER_GLFW)
    CompileDriver(DRIVER_GLFW
            glfw
            glfw::GLFWDisplayDriver
            glfw)
endif ()

if (DRIVER_GLFW_OPENGL)
    add_compile_definitions(DRIVER_GLFW_OPENGL)
endif ()

if (DRIVER_GLFW_VULKAN)
    add_compile_definitions(DRIVER_GLFW_VULKAN)
endif ()

if (DRIVER_ANDROID)
    CompileDriver(DRIVER_ANDROID
            android
            android::AndroidDisplayDriver
            android
            EGL)
endif ()

if (DRIVER_ANDROID_OPENGL)
    add_compile_definitions(DRIVER_ANDROID_OPENGL)
endif ()

if (DRIVER_OPENGL)
    if (ANDROID)
        set(GL_LIBNAME GLESv3)
    elseif (WIN32)
        set(GL_LIBNAME OpenGL32)
    else ()
        set(GL_LIBNAME GL)
    endif ()
    CompileDriver(DRIVER_OPENGL
            opengl
            opengl::OGLGpuDriver
            ${GL_LIBNAME})
endif ()

if (DRIVER_VULKAN)
    find_package(Vulkan REQUIRED)
    include_directories(${Vulkan_INCLUDE_DIRS})
    CompileDriver(DRIVER_VULKAN
            vulkan
            vulkan::VkGpuDriver
            ${Vulkan_LIBRARIES})
endif ()

if (DRIVER_BOX2D)
    CompileDriver(DRIVER_BOX2D
            box2d
            PhysicsDriverBox2D
            box2d)
endif ()

if (DRIVER_BULLET3)
    CompileDriver(DRIVER_BULLET3
            bullet3
            PhysicsDriverBt3
            BulletDynamics BulletCollision LinearMath)
endif ()

if (DRIVER_OPENAL)
    CompileDriver(DRIVER_OPENAL
            openal-soft
            OALAudioDriver
            OpenAL)
endif ()

if (DRIVER_FREETYPE)
    CompileDriver(DRIVER_FREETYPE
            freetype
            FtFontDriver
            freetype)
endif ()

if (DRIVER_ASSIMP)
    CompileDriver(DRIVER_ASSIMP
            assimp
            AssImpParser
            assimp)
endif ()

if (DRIVER_SNDFILE)
    CompileDriver(DRIVER_SNDFILE
            sndfile
            SndFileParser
            sndfile)
endif ()

if (DRIVER_GLSLANG)
    CompileDriver(DRIVER_GLSLANG
            glslang
            GlslangCompiler
            glslang SPIRV glslang-default-resource-limits MachineIndependent OSDependent GenericCodeGen OGLCompiler)
endif ()

if (DRIVER_SPIRVCROSS)
    CompileDriver(DRIVER_SPIRVCROSS
            spirv-cross
            SpirvCrossDecompiler
            spirv-cross-core spirv-cross-glsl spirv-cross-hlsl)
endif ()

if (DRIVER_CRYPTOPP)
    CompileDriver(DRIVER_CRYPTOPP
            cryptopp
            CryptoPPDriver
            cryptopp)
endif ()

### --  Stop Driver Definitions -- ###

set(STR_LIBRARIES "")
foreach (val IN LISTS DRIVERS_LINK)
    set(STR_LIBRARIES "${STR_LIBRARIES}${val} ")
endforeach ()

list(LENGTH DRIVERS_INCLUDE LEN_DRIVERS_INCLUDE)

message("${LEN_DRIVERS_INCLUDE} Drivers Loaded")
foreach (val IN LISTS DRIVERS_INCLUDE)
    message("   ${val}")
endforeach ()

if (${LEN_DRIVERS_INCLUDE} GREATER 0)
    message("Linked Libraries: ${STR_LIBRARIES}")
endif ()

file(GLOB_RECURSE DRIVERS_SRC ${Drivers.GLOBEXPR})
