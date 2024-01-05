cmake_minimum_required(VERSION 3.7)
project(CSLP_Project)

add_compile_options(-Ofast -march=native)

## Libraries
### BitStream
add_library(BitStream BitStream.cpp
        Golomb.cpp
)