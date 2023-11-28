cmake_minimum_required(VERSION 3.7)
project(CSLP_Project)

# Targets
add_compile_options(-Ofast -march=native -mtune=native)
## Libraries
add_library(Codec
        Frame.cpp
        Header.hpp
        Header.cpp
        encoders/LosslessIntra.cpp
        encoders/LosslessInter.cpp
        encoders/LosslessHybrid.cpp
        encoders/LossyHybrid.hpp
        encoders/LossyIntra.hpp
        encoders/LossyInter.hpp
        encoders/LossyHybrid.cpp
        encoders/LossyInter.cpp
        encoders/LossyIntra.cpp
        Quantizer.cpp
        Quantizer.hpp
)
if (OpenMP_CXX_FOUND)
    target_link_libraries(Codec PUBLIC OpenMP::OpenMP_CXX)
endif ()