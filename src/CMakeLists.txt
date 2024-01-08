cmake_minimum_required(VERSION 3.7)
project(CSLP_src)

add_subdirectory(visual)
add_subdirectory(io)
add_subdirectory(codec)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../../../bin")
add_subdirectory(cli)