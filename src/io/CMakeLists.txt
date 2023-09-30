cmake_minimum_required(VERSION 3.7)
project(CSLP_io)

# Targets

## Libraries
### BitStream
add_library(BitStream BitStream.hpp)

set_target_properties(BitStream PROPERTIES LINKER_LANGUAGE CXX)