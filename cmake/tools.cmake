add_executable(headertool ${CMAKE_CURRENT_SOURCE_DIR}/tools/headertool/main.cpp)
target_include_directories(headertool PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tools/headertool)
target_link_libraries(headertool Threads::Threads xengine)

add_executable(scenetool ${CMAKE_CURRENT_SOURCE_DIR}/tools/scenetool/main.cpp)
target_include_directories(scenetool PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tools/scenetool)
target_link_libraries(scenetool Threads::Threads xengine)