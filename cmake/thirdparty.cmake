function(add_thirdparty_subdir PATH)
    file(GLOB RESULT ${PATH}*)
    list(LENGTH RESULT RES_LEN)
    if (RES_LEN GREATER 0)
        add_subdirectory(${PATH})
    else ()
        message(WARNING "Third Party submodule ${PATH} not found. Please download the submodule, provide the corresponding dependency yourself or disable the driver.")
    endif ()
endfunction()

function(add_thirdparty_include PATH)
    file(GLOB RESULT ${PATH}*)
    list(LENGTH RESULT RES_LEN)
    if (RES_LEN GREATER 0)
        include_directories(${PATH})
    else ()
        message(WARNING "Third Party submodule ${PATH} not found. Please download the submodule or provide the corresponding dependency yourself.")
    endif ()
endfunction()

set(THIRD_PARTY_BASE ${CMAKE_SOURCE_DIR}/thirdparty)

if (DRIVER_GLFW)
    add_thirdparty_subdir(${THIRD_PARTY_BASE}/glfw/)
endif ()

if (DRIVER_BOX2D)
    set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
    set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "" FORCE)
    add_thirdparty_subdir(${THIRD_PARTY_BASE}/box2d/)
endif ()

if (DRIVER_BULLET3)
    #  add_thirdparty_subdir(${THIRD_PARTY_BASE}/bullet3/)
endif ()

if (DRIVER_OPENAL)
    set(LIBTYPE STATIC CACHE STRING "" FORCE)
    add_thirdparty_subdir(${THIRD_PARTY_BASE}/openal-soft/)
endif ()

if (DRIVER_FREETYPE)
    add_thirdparty_subdir(${THIRD_PARTY_BASE}/freetype/)
endif ()

if (DRIVER_ASSIMP)
    add_thirdparty_subdir(${THIRD_PARTY_BASE}/assimp/)
endif ()

if (DRIVER_SNDFILE)
    add_thirdparty_subdir(${THIRD_PARTY_BASE}/libsndfile/)
endif ()

if (DRIVER_SPIRVCROSS)
    add_thirdparty_subdir(${THIRD_PARTY_BASE}/SPIRV-Cross/)
endif ()

if (DRIVER_CRYPTOPP)
    add_thirdparty_subdir(${THIRD_PARTY_BASE}/cryptopp/)
endif ()

set(ENABLE_OPT OFF CACHE BOOL "" FORCE)
add_thirdparty_subdir(${THIRD_PARTY_BASE}/glslang/)

add_thirdparty_include(${THIRD_PARTY_BASE}/glm/)