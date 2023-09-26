cmake_minimum_required(VERSION 3.7)
project(CSLP_lib)

add_library(Image Image.hpp)

target_link_libraries(Image ${OpenCV_LIBS})
set_target_properties(Image PROPERTIES LINKER_LANGUAGE CXX)