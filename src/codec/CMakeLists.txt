cmake_minimum_required(VERSION 3.7)
project(CSLP_Project)

# Targets
add_compile_options(-O2 -Wall -Wextra -Wpedantic)
## Libraries
add_library(Codec
        Frame.cpp
)
