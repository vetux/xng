# Windows / Linux Host build
- Download the third party submodules (eg. <code>git submodule update</code>) or provide the necessary dependencies yourself by creating <code>user-config-project.cmake</code> and setting include_directories and link_directories respectively.
- (Optional) Enable / Disable the desired [drivers](cmake/drivers.cmake) in the <code>user-config-project.cmake</code> file. (Eg. <code>set(DRIVER_* ON/OFF)</code> to override the options)
- Open CMakeLists.txt in your favorite IDE or with the cmake command
- Build the desired cmake targets

# Android Cross Compile

# Windows Cross Compile - MinGW

