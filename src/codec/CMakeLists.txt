cmake_minimum_required(VERSION 3.7)
project(CSLP_Project)

## Libraries
add_library(Codec
        Frame.cpp
        Header.hpp
        Header.cpp
        encoders/lossless/LosslessIntra.cpp
        encoders/lossless/LosslessInter.cpp
        encoders/lossless/LosslessHybrid.cpp
        encoders/lossy/LossyHybrid.cpp
        encoders/lossy/LossyIntra.cpp
        encoders/lossy/DCTEncoder.cpp
        encoders/RLEEncoder.cpp
        Quantizer.cpp
)

target_link_libraries(Codec ${OpenCV_LIBS})