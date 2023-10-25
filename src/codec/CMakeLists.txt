cmake_minimum_required(VERSION 3.7)
project(CSLP_io)

# Targets

## Libraries
add_library(codec
        Frame.hpp
        Frame.cpp
)

set_target_properties(codec PROPERTIES LINKER_LANGUAGE CXX)