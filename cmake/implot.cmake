include_directories(submodules/implot/)

file(GLOB IMPLOT.SRC submodules/implot/*.cpp)

add_library(implot STATIC ${IMPLOT.SRC})