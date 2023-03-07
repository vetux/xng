# Build Configuration
Create a file in this directory called <code>user-config-toolchain.cmake</code> and set the desired build configuration.
Supported values are:
- USER_CONF_ANDROID_NDK - The path to the android ndk
- USER_CONF_ANDROID_SYSTEM_VERSION - The target android version
- USER_CONF_ANDROID_STL_TYPE - The Standard library type

For each ABI you can create a file called <code>user-config-project.cmake</code> inside the corresponding ABI folder
to run cmake commands for the abi project to specify driver dependency headers and link directories per ABI.