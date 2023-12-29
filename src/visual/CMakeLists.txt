cmake_minimum_required(VERSION 3.7)
project(CSLP_Project)

# Targets

## Libraries
### Image
add_library(Visual Image.cpp
        ImageProcessing.cpp
        Video.cpp
        YuvParser.cpp
)

target_link_libraries(Visual ${OpenCV_LIBS})