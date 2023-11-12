option(SUBMODULE_DEPENDENCIES "Use the submodules to satisfy driver dependencies" true)

if (SUBMODULE_DEPENDENCIES)
    add_subdirectory(${CMAKE_SOURCE_DIR}/submodules/glfw/)

    set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
    set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "" FORCE)
    add_subdirectory(${CMAKE_SOURCE_DIR}/submodules/box2d/)

    #  add_subdirectory(${CMAKE_SOURCE_DIR}/submodules/bullet3/)

    set(LIBTYPE STATIC CACHE STRING "" FORCE)
    add_subdirectory(${CMAKE_SOURCE_DIR}/submodules/openal-soft/)

    add_subdirectory(${CMAKE_SOURCE_DIR}/submodules/freetype/)

    add_subdirectory(${CMAKE_SOURCE_DIR}/submodules/assimp/)

    add_subdirectory(${CMAKE_SOURCE_DIR}/submodules/libsndfile/)

    set(ENABLE_OPT OFF CACHE BOOL "" FORCE)
    add_subdirectory(${CMAKE_SOURCE_DIR}/submodules/glslang/)

    add_subdirectory(${CMAKE_SOURCE_DIR}/submodules/SPIRV-Cross/)

    add_subdirectory(${CMAKE_SOURCE_DIR}/submodules/cryptopp/)

    include_directories(${CMAKE_SOURCE_DIR}/submodules/glm/)
endif ()