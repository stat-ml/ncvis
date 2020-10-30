add_library(table INTERFACE)
target_include_directories(table INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(table INTERFACE tbb_static)