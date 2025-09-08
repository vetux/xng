# Windows / Linux Host build
- Download the third party submodules (eg. <code>git submodule update</code>) or provide the necessary dependencies yourself by creating <code>user-config-project.cmake</code> and setting include_directories and link_directories respectively.
- (Optional) Enable / Disable the desired [adapters](cmake/adapters.cmake) in the <code>user-config-project.cmake</code> file. (Eg. <code>set(BUILD_* ON/OFF CACHE BOOL "" FORCE)</code>to override the options)
- Open CMakeLists.txt in your favorite IDE or directly with CMake
- Build the desired cmake targets

# Android / MinGW Cross Compile
- Build the xengine on the host to generate the compiled shaders for the cross compile build.
- Download the third party submodules (eg. git submodule update) or provide the necessary dependencies yourself by creating user-config-project.cmake and setting include_directories and link_directories respectively.
- (Optional) Enable / Disable the desired adapters in the user-config-project.cmake file. (Eg. <code>set(BUILD_* ON/OFF CACHE BOOL "" FORCE)</code> to override the options)
- Run Cmake with the desired toolchain in [cmake/toolchains/](cmake/toolchains/) eg <code> cmake ./ -B build/ -DCMAKE_TOOLCHAIN_FILE=./cmake/toolchains/android-armeabi-v7a.cmake -DCMAKE_SYSTEM_VERSION=26 -DCROSS_COMPILING=""</code>
- Run make in the generated build directory eg <code> cd build/ && make </code>
