include(ExternalProject)

ExternalProject_Add(build-mingw-all
        INSTALL_COMMAND ""
        SOURCE_DIR ${BASE_SOURCE_DIR}/platforms/mingw/
        CMAKE_ARGS
        -DBASE_SOURCE_DIR=${BASE_SOURCE_DIR}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        )

ExternalProject_Add(build-mingw-x86
        INSTALL_COMMAND ""
        SOURCE_DIR ${BASE_SOURCE_DIR}/platforms/mingw/x86-win32
        CMAKE_ARGS
        -DCMAKE_TOOLCHAIN_FILE=${BASE_SOURCE_DIR}/platforms/mingw/x86-win32/windows-x86.cmake
        -DBASE_SOURCE_DIR=${BASE_SOURCE_DIR}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        )

ExternalProject_Add(build-mingw-x64
        INSTALL_COMMAND ""
        SOURCE_DIR ${BASE_SOURCE_DIR}/platforms/mingw/x86_64-win32
        CMAKE_ARGS
        -DCMAKE_TOOLCHAIN_FILE=${BASE_SOURCE_DIR}/platforms/mingw/x86_64-win32/windows-x64.cmake
        -DBASE_SOURCE_DIR=${BASE_SOURCE_DIR}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        )
