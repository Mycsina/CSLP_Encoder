cmake_minimum_required(VERSION 3.7)
project(CSLP_Project)

## Libraries
### Image
add_library(Visual Image.cpp
        ImageProcessing.cpp
        Video.cpp
        YuvParser.cpp
        YuvWriter.cpp
)

target_link_libraries(Visual ${OpenCV_LIBS})