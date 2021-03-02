add_library(pcg INTERFACE)
target_include_directories(pcg INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/pcg-cpp/include>
)

add_subdirectory(table)