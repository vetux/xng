# The platform specific global build settings, users should only need to edit this file to be able to build on the target platform

# Defaults
include_directories(${PROJECT_SOURCE_DIR}/lib/include)
link_directories(${PROJECT_SOURCE_DIR}/lib/bin)
set(BOX2D_VERSION 0) # The Box2d version number
set(BOX2D_LIB "box2d") # The Box2d library name

# Debian 11 bullseye
include_directories(/usr/include/freetype2/ /usr/include/mono-2.0/)
set(BOX2D_VERSION 231)
set(BOX2D_LIB "Box2D")
