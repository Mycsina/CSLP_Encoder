cmake_minimum_required(VERSION 3.7)
project(CSLP_Project)

# Targets

## Libraries
### BitStream
add_library(BitStream BitStream.cpp
        Golomb.cpp
)