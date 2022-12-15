add_executable(headertool ${CMAKE_CURRENT_SOURCE_DIR}/tools/headertool/main.cpp)

target_include_directories(headertool PRIVATE ${HeaderTool.Dir.SRC})

target_link_libraries(headertool Threads::Threads xengine)