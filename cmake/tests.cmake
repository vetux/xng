set(TESTS_COMMON_DIR ${BASE_SOURCE_DIR}/tests/common/src/)
set(TESTS_ASSET_DIR ${BASE_SOURCE_DIR}/tests/assets)

add_executable(test-pak ${BASE_SOURCE_DIR}/tests/pak/src/main.cpp)
target_include_directories(test-pak PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/pak/src/ ${TESTS_COMMON_DIR})
target_link_libraries(test-pak Threads::Threads xengine)

add_executable(benchmark-renderer ${BASE_SOURCE_DIR}/tests/benchmark-renderer/src/main.cpp)
target_include_directories(benchmark-renderer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests/benchmark-renderer/src/ ${TESTS_COMMON_DIR})
target_link_libraries(benchmark-renderer Threads::Threads xengine)

if (MSVC)
    target_compile_options(test-pak PUBLIC /bigobj)
    target_compile_options(benchmark-renderer PUBLIC /bigobj)
endif ()

file(GLOB RESULT ${TESTS_ASSET_DIR}/*)
list(LENGTH RESULT RES_LEN)
if (RES_LEN GREATER 0)
    file(COPY ${TESTS_ASSET_DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) # Copy assets
else ()
    message(WARNING "Test assets submodule not found, please download the assets submodule if you intend to run the tests.")
endif ()
