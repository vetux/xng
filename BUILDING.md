# Windows / Linux Host build

- Create a file called <code>user-config-project.cmake</code> in the [cmake](cmake) directory and
  set the link_directories and include_directories for the driver dependencies. GLM headers must currently be provided for building the xEngine (GLM dependency will be
  removed in the future)
- Enable / Disable the desired drivers [cmake/drivers.cmake](cmake/drivers.cmake)
- Open CMakeLists.txt in your favorite IDE or with the cmake command
- Build the desired cmake targets

# Android Cross Compile

- Create a file called <code>user-config-toolchain.cmake</code> in the [toolchains/](cmake/toolchains) directory and set the USER_CONF_ANDROID_NDK path to
  point to your Android NDK installation folder or set the required environment variables by cmake. Available options are:
  - USER_CONF_ANDROID_NDK - The path to the android ndk
  - USER_CONF_ANDROID_SYSTEM_VERSION - The target android version
  - USER_CONF_ANDROID_STL_TYPE - The Standard library type
- Create a file called <code>user-config-project.cmake</code> in the [cmake](cmake) directory and
  set the link_directories and include_directories for the driver dependencies. GLM headers must currently be provided for building the xEngine (GLM dependency will be
  removed in the future)
- Enable / Disable the desired drivers [cmake/drivers.cmake](cmake/drivers.cmake)
- Open [CMakeLists.txt](CMakeLists.txt) in your favorite IDE and add -DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain to the
  command line arguments. /path/to/toolchain points to the [toolchain](cmake/toolchains) file for the desired ABI
    - CLion: Create a new CMake Profile with the required cmake argument (Settings -> Build, Execution, Deployment ->
      CMake) for each ABI
- Build the desired cmake targets

# Notes

## Box2D

The box2d physics driver requires version 2.4.1 of box2d. Make sure to not have conflicting versions of the library on
the include / link paths.
