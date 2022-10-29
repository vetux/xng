option(DRIVER_GLFW "Build the glfw display driver" ON)
option(DRIVER_OPENGL "Build the OpenGL render driver (Window and Render implementations" ON)
option(DRIVER_MONO "Build the mono script driver" OFF)
option(DRIVER_BOX2D "Build the box2d physics driver" ON)
option(DRIVER_BULLET3 "Build the bullet3 physics driver" ON)
option(DRIVER_OPENAL "Build the OpenAL audio driver" ON)
option(DRIVER_FREETYPE "Build the FreeType font rendering driver" ON)
option(DRIVER_ASSIMP "Build the AssImp resource parser driver (For 3D asset file formats)" ON)
option(DRIVER_SNDFILE "Build the SndFile resource parser driver (For Audio file formats)" ON)
option(DRIVER_SHADERC "Build the ShaderC shader compiler driver" ON)
option(DRIVER_SPIRVCROSS "Build the SPIRV-Cross shader decompiler driver" ON)
option(DRIVER_CRYPTOPP "Build the CryptoPP driver" ON)

set(DRIVERS_INCLUDE) # The drivers include directories in a list
set(DRIVERS_SRC) # The drivers source files in a list
set(DRIVERS_LINK) # The drivers linked library names in a list
set(Drivers.GLOBEXPR) # The globexpr used to generate DRIVERS_SRC

# @DIR = The directory in drivers/ which contains the driver source
# @COMPILE_DEFS = Compile definitions
# @DRIVER_LINK = The library name/s which the driver links to. There can be multiple DRIVER_LINK arguments.
function(CompileDriver COMPILE_DEFS DIR)
    set(Drivers.GLOBEXPR ${Drivers.GLOBEXPR} drivers/${DIR}/*.cpp drivers/${DIR}/*.c PARENT_SCOPE)
    add_compile_definitions(${COMPILE_DEFS})
    set(DRIVERS_INCLUDE ${DRIVERS_INCLUDE} drivers/${DIR}/ PARENT_SCOPE)
    if (${ARGC} GREATER 2)
        # Each additional argument is treated as a library name
        set(MAXINDEX ${ARGC})
        MATH(EXPR MAXINDEX "${MAXINDEX}-1")
        foreach (index RANGE 2 ${MAXINDEX})
            list(GET ARGV ${index} LIBNAME)
            set(DRIVER_LINK ${DRIVER_LINK} ${LIBNAME})
        endforeach ()
        set(DRIVERS_LINK ${DRIVERS_LINK} ${DRIVER_LINK} PARENT_SCOPE)
    endif ()
endfunction()

### --  Start Driver Definitions -- ###

if (DRIVER_GLFW)
    CompileDriver(DRIVER_GLFW glfw glfw)
endif ()

if (DRIVER_OPENGL)
    if (WIN32)
        set(GL_LIBNAME OpenGL32)
    else ()
        set(GL_LIBNAME GL)
    endif ()
    CompileDriver(DRIVER_OPENGL opengl ${GL_LIBNAME})
endif ()

if (DRIVER_MONO)
    CompileDriver(DRIVER_MONO mono mono-2.0)
endif ()

if (DRIVER_BOX2D)
    add_compile_definitions(BOX2D_VERSION=${BOX2D_VERSION})
    CompileDriver(DRIVER_BOX2D box2d ${BOX2D_LIB})
endif ()

if (DRIVER_BULLET3)
    CompileDriver(DRIVER_BULLET3 bullet3)
endif ()

if (DRIVER_OPENAL)
    if (WIN32)
        set(AL_LIBNAME OpenAL32)
    else ()
        set(AL_LIBNAME openal)
    endif ()
    CompileDriver(DRIVER_OPENAL openal ${AL_LIBNAME})
endif ()

if (DRIVER_FREETYPE)
    CompileDriver(DRIVER_FREETYPE freetype freetype)
endif ()

if (DRIVER_ASSIMP)
    CompileDriver(DRIVER_ASSIMP assimp assimp)
endif ()

if (DRIVER_SNDFILE)
    CompileDriver(DRIVER_SNDFILE sndfile sndfile)
endif ()

if (DRIVER_SHADERC)
    CompileDriver(DRIVER_SHADERC shaderc shaderc_combined)
endif ()

if (DRIVER_SPIRVCROSS)
    CompileDriver(DRIVER_SPIRVCROSS spirv-cross spirv-cross-core spirv-cross-glsl spirv-cross-hlsl)
endif ()

if (DRIVER_CRYPTOPP)
    CompileDriver(DRIVER_CRYPTOPP cryptopp cryptopp)
endif ()

### --  Stop Driver Definitions -- ###

set(STR_LIBRARIES "")
foreach (val IN LISTS DRIVERS_LINK)
    set(STR_LIBRARIES "${STR_LIBRARIES}${val} ")
endforeach ()

list(LENGTH DRIVERS_INCLUDE LEN_DRIVERS_INCLUDE)

message("${LEN_DRIVERS_INCLUDE} Drivers Loaded :")
foreach (val IN LISTS DRIVERS_INCLUDE)
    message("   ${val}")
endforeach ()

message("Linked Libraries: ${STR_LIBRARIES}")

file(GLOB_RECURSE DRIVERS_SRC ${Drivers.GLOBEXPR})