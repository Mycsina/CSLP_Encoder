cmake_minimum_required(VERSION 3.7)
project(CSLP_Project)

add_executable(CSLPEncoder
        CSLPEncoder.cpp
)

target_link_libraries(CSLPEncoder
        ${OpenCV_LIBS}
        Codec
        Visual
        BitStream
)
