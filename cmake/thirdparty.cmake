function(add_thirdparty_subdir PATH)
    file(GLOB RESULT ${PATH}*)
    list(LENGTH RESULT RES_LEN)
    if (RES_LEN GREATER 0)
        message("Adding submodule directory ${PATH}")
        add_subdirectory(${PATH})
    else ()
        message(WARNING "Third Party submodule ${PATH} not found. Please download the git submodule or provide the corresponding dependency yourself.")
    endif ()
endfunction()

function(add_thirdparty_include PATH)
    file(GLOB RESULT ${PATH}*)
    list(LENGTH RESULT RES_LEN)
    if (RES_LEN GREATER 0)
        message("Adding submodule include ${PATH}")
        include_directories(${PATH})
    else ()
        message(WARNING "Third Party submodule ${PATH} not found. Please download the git submodule or provide the corresponding dependency yourself.")
    endif ()
endfunction()

set(THIRD_PARTY_BASE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty)

add_thirdparty_subdir(${THIRD_PARTY_BASE}/glfw/)

add_thirdparty_subdir(${THIRD_PARTY_BASE}/SPIRV-Cross/)

set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "" FORCE)
add_thirdparty_subdir(${THIRD_PARTY_BASE}/box2d/)

set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
add_thirdparty_subdir(${THIRD_PARTY_BASE}/bullet3/)
add_thirdparty_include(${THIRD_PARTY_BASE}/bullet3/src/)

set(LIBTYPE STATIC CACHE STRING "" FORCE)
add_thirdparty_subdir(${THIRD_PARTY_BASE}/openal-soft/)

add_thirdparty_subdir(${THIRD_PARTY_BASE}/freetype/)

set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
add_thirdparty_subdir(${THIRD_PARTY_BASE}/assimp/)

add_thirdparty_subdir(${THIRD_PARTY_BASE}/libsndfile/)

add_thirdparty_subdir(${THIRD_PARTY_BASE}/cryptopp/)

set(ENABLE_OPT OFF CACHE BOOL "" FORCE)
add_thirdparty_subdir(${THIRD_PARTY_BASE}/glslang/)

add_thirdparty_include(${THIRD_PARTY_BASE}/glm/)

set(BUILD_FUZZ_TESTS OFF CACHE BOOL "" FORCE)
add_thirdparty_subdir(${THIRD_PARTY_BASE}/yoga/)