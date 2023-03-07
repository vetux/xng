include(ExternalProject)

ExternalProject_Add(build-android-all
        INSTALL_COMMAND ""
        SOURCE_DIR ${BASE_SOURCE_DIR}/platforms/android/toolchains/
        CMAKE_ARGS
        ${GLOBAL_DEFAULT_ARGS}
        ${GLOBAL_THIRDPARTY_LIB_ARGS}
        -DBASE_SOURCE_DIR=${BASE_SOURCE_DIR}
        )

ExternalProject_Add(build-android-arm64-v8a
        INSTALL_COMMAND ""
        SOURCE_DIR ${BASE_SOURCE_DIR}/platforms/android/toolchains/arm64-v8a
        CMAKE_ARGS
        ${GLOBAL_DEFAULT_ARGS}
        ${GLOBAL_THIRDPARTY_LIB_ARGS}
        -DCMAKE_TOOLCHAIN_FILE=${BASE_SOURCE_DIR}/platforms/android/toolchains/arm64-v8a/android-arm64-v8a.cmake
        -DBASE_SOURCE_DIR=${BASE_SOURCE_DIR}
        )

ExternalProject_Add(build-android-armeabi-v7a
        INSTALL_COMMAND ""
        SOURCE_DIR ${BASE_SOURCE_DIR}/platforms/android/toolchains/armeabi-v7a
        CMAKE_ARGS
        ${GLOBAL_DEFAULT_ARGS}
        ${GLOBAL_THIRDPARTY_LIB_ARGS}
        -DCMAKE_TOOLCHAIN_FILE=${BASE_SOURCE_DIR}/platforms/android/toolchains/armeabi-v7a/android-armeabi-v7a.cmake
        -DBASE_SOURCE_DIR=${BASE_SOURCE_DIR}
        )

ExternalProject_Add(build-android-x86
        INSTALL_COMMAND ""
        SOURCE_DIR ${BASE_SOURCE_DIR}/platforms/android/toolchains/x86
        CMAKE_ARGS
        ${GLOBAL_DEFAULT_ARGS}
        ${GLOBAL_THIRDPARTY_LIB_ARGS}
        -DCMAKE_TOOLCHAIN_FILE=${BASE_SOURCE_DIR}/platforms/android/toolchains/x86/android-x86.cmake
        -DBASE_SOURCE_DIR=${BASE_SOURCE_DIR}
        )

ExternalProject_Add(build-android-x86_64
        INSTALL_COMMAND ""
        SOURCE_DIR ${BASE_SOURCE_DIR}/platforms/android/toolchains/x86_64
        CMAKE_ARGS
        ${GLOBAL_DEFAULT_ARGS}
        ${GLOBAL_THIRDPARTY_LIB_ARGS}
        -DCMAKE_TOOLCHAIN_FILE=${BASE_SOURCE_DIR}/platforms/android/toolchains/x86_64/android-x86_64.cmake
        -DBASE_SOURCE_DIR=${BASE_SOURCE_DIR}
        )
