cmake_minimum_required(VERSION 3.7)
project(CSLP_Project)

# Targets
add_compile_options(-O3 -D_VISUALIZE)
## Libraries
add_library(Codec
        Frame.cpp
        LosslessIntra.cpp
)
if (OpenMP_CXX_FOUND)
    target_link_libraries(Codec PUBLIC OpenMP::OpenMP_CXX)
endif ()