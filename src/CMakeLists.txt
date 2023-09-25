cmake_minimum_required(VERSION 3.7)
project(openCVTest)

# Define project name
project(project)
find_package(OpenCV REQUIRED)

message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

add_library(Image Image.cpp)

target_link_libraries(Image ${OpenCV_LIBS})
