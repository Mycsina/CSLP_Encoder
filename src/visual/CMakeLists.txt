cmake_minimum_required(VERSION 3.7)
project(CSLP_Project)

# Targets
add_compile_options(-Ofast -march=native)

## Libraries
### Image
add_library(Visual Image.cpp
        ImageProcessing.cpp
        Video.cpp
        YuvParser.cpp
        YuvWriter.cpp
)

target_link_libraries(Visual ${OpenCV_LIBS})