add_executable(test-framegraph ${BASE_SOURCE_DIR}/tests/framegraph/src/main.cpp)
target_include_directories(test-framegraph PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/framegraph/src/)
target_link_libraries(test-framegraph Threads::Threads xengine)

add_executable(test-renderer2d ${BASE_SOURCE_DIR}/tests/renderer2d/src/main.cpp)
target_include_directories(test-renderer2d PRIVATE ${CMAKE_SOURCE_DIR}/tests/renderer2d/src/)
target_link_libraries(test-renderer2d Threads::Threads xengine)

add_executable(test-canvasrendersystem ${BASE_SOURCE_DIR}/tests/canvasrendersystem/src/main.cpp)
target_include_directories(test-canvasrendersystem PRIVATE ${CMAKE_SOURCE_DIR}/tests/canvasrendersystem/src/)
target_link_libraries(test-canvasrendersystem Threads::Threads xengine)

add_executable(test-pak ${BASE_SOURCE_DIR}/tests/pak/src/main.cpp)
target_include_directories(test-pak PRIVATE ${CMAKE_SOURCE_DIR}/tests/pak/src/)
target_link_libraries(test-pak Threads::Threads xengine)

file(COPY ${BASE_SOURCE_DIR}/tests/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) # Copy assets