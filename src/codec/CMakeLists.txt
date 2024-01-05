cmake_minimum_required(VERSION 3.7)
project(CSLP_Project)

add_compile_options(-Ofast -march=native)

## Libraries
add_library(Codec
        Frame.cpp
        Header.hpp
        Header.cpp
        encoders/LosslessIntra.cpp
        encoders/LosslessInter.cpp
        encoders/LosslessHybrid.cpp
        encoders/LossyHybrid.cpp
        encoders/LossyInter.cpp
        encoders/LossyIntra.cpp
        Quantizer.cpp
)

if (OpenMP_CXX_FOUND)
    target_link_libraries(Codec PUBLIC OpenMP::OpenMP_CXX)
endif ()