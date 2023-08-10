set(TESTS_COMMON_DIR ${BASE_SOURCE_DIR}/tests/common/src/)
set(TESTS_ASSET_DIR ${BASE_SOURCE_DIR}/tests/common/assets)

add_executable(test-framegraph ${BASE_SOURCE_DIR}/tests/framegraph/src/main.cpp)
target_include_directories(test-framegraph PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/framegraph/src/ ${BASE_SOURCE_DIR}/shaders/generated/ ${TESTS_COMMON_DIR})
target_link_libraries(test-framegraph Threads::Threads xengine)

add_executable(test-renderer2d ${BASE_SOURCE_DIR}/tests/renderer2d/src/main.cpp)
target_include_directories(test-renderer2d PRIVATE ${CMAKE_SOURCE_DIR}/tests/renderer2d/src/ ${TESTS_COMMON_DIR})
target_link_libraries(test-renderer2d Threads::Threads xengine)

add_executable(test-canvasrendersystem ${BASE_SOURCE_DIR}/tests/canvasrendersystem/src/main.cpp)
target_include_directories(test-canvasrendersystem PRIVATE ${CMAKE_SOURCE_DIR}/tests/canvasrendersystem/src/ ${TESTS_COMMON_DIR})
target_link_libraries(test-canvasrendersystem Threads::Threads xengine)

add_executable(test-pak ${BASE_SOURCE_DIR}/tests/pak/src/main.cpp)
target_include_directories(test-pak PRIVATE ${CMAKE_SOURCE_DIR}/tests/pak/src/ ${TESTS_COMMON_DIR})
target_link_libraries(test-pak Threads::Threads xengine)

add_executable(test-mandelbrot ${BASE_SOURCE_DIR}/tests/mandelbrot/src/main.cpp)
target_include_directories(test-mandelbrot PRIVATE ${CMAKE_SOURCE_DIR}/tests/mandelbrot/src/ ${BASE_SOURCE_DIR}/shaders/generated/ ${TESTS_COMMON_DIR})
target_link_libraries(test-mandelbrot Threads::Threads xengine)

if (MSVC)
	target_compile_options(test-canvasrendersystem PUBLIC /bigobj)
endif ()

file(COPY ${TESTS_ASSET_DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) # Copy assets
