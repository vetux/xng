# Windows / Linux Host Build
- Download the third party submodules (eg. `git submodule update --init --recursive`) or provide the necessary dependencies yourself.
- Open CMakeLists.txt in your favorite IDE or directly with CMake / CMake GUI
- (Optional) Enable / Disable the desired adapters by setting the corresponding BUILD_* CMake options ([cmake/adapters.cmake](cmake/adapters.cmake))
- Build the desired targets (Third party dependencies are automatically compiled from source if the corresponding submodules are present)

# Android / MinGW Cross Compile Additional Build Steps
- Run Cmake with the desired toolchain in [cmake/toolchains/](cmake/toolchains/) eg <code> cmake ./ -B build/ -DCMAKE_TOOLCHAIN_FILE=./cmake/toolchains/android-armeabi-v7a.cmake -DCMAKE_SYSTEM_VERSION=26 -DCROSS_COMPILING=""</code>
- Run make in the generated build directory eg <code> cd build/ && make </code>
