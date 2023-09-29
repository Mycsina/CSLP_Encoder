cmake_minimum_required(VERSION 3.7)
project(CSLP_lib)

# Targets

## Libraries
### Image
add_library(Image image.hpp
        imageProcessing.cpp
        imageProcessing.hpp)

target_link_libraries(Image ${OpenCV_LIBS})
set_target_properties(Image PROPERTIES LINKER_LANGUAGE CXX)

### Video
add_library(Video video.hpp
)

target_link_libraries(Video ${OpenCV_LIBS})
set_target_properties(Video PROPERTIES LINKER_LANGUAGE CXX)