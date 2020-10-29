add_library(simplexvis STATIC simplicialset.cpp simplicialset.hpp)
add_executable(simplexvis_test main.cpp)

# target_link_libraries(simplexvis_test PUBLIC simplexvis)
if(OpenMP_CXX_FOUND)
    target_link_libraries(simplexvis PUBLIC OpenMP::OpenMP_CXX)
endif()