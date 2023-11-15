option(SUBMODULE_DEPENDENCIES "Use the submodules to satisfy driver dependencies" ON)

if (SUBMODULE_DEPENDENCIES)
    if (DRIVER_GLFW)
        add_subdirectory(${CMAKE_SOURCE_DIR}/submodules/glfw/)
    endif ()

    if (DRIVER_BOX2D)
        set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
        set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "" FORCE)
        add_subdirectory(${CMAKE_SOURCE_DIR}/submodules/box2d/)
    endif ()

    if (DRIVER_BULLET3)
        #  add_subdirectory(${CMAKE_SOURCE_DIR}/submodules/bullet3/)
    endif ()

    if (DRIVER_OPENAL)
        set(LIBTYPE STATIC CACHE STRING "" FORCE)
        add_subdirectory(${CMAKE_SOURCE_DIR}/submodules/openal-soft/)
    endif ()

    if (DRIVER_FREETYPE)
        add_subdirectory(${CMAKE_SOURCE_DIR}/submodules/freetype/)
    endif ()

    if (DRIVER_ASSIMP)
        add_subdirectory(${CMAKE_SOURCE_DIR}/submodules/assimp/)
    endif ()

    if (DRIVER_SNDFILE)
        add_subdirectory(${CMAKE_SOURCE_DIR}/submodules/libsndfile/)
    endif ()

    if (DRIVER_SPIRVCROSS)
        add_subdirectory(${CMAKE_SOURCE_DIR}/submodules/SPIRV-Cross/)
    endif ()

    if (DRIVER_CRYPTOPP)
        add_subdirectory(${CMAKE_SOURCE_DIR}/submodules/cryptopp/)
    endif ()

    set(ENABLE_OPT OFF CACHE BOOL "" FORCE)
    add_subdirectory(${CMAKE_SOURCE_DIR}/submodules/glslang/)

    include_directories(${CMAKE_SOURCE_DIR}/submodules/glm/)
endif ()