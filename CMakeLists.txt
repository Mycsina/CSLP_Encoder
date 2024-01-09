cmake_minimum_required(VERSION 3.14)
project(CSLP_Project)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Debug mode")
    add_compile_options(-g -Wall -Wextra -pedantic)
else ()
  message("Release mode")
  add_compile_options(-Ofast -march=native)
endif ()

# CMake configuration
## Avoid compile warning CMP0135
cmake_policy(SET CMP0135 NEW)

# Dependencies
## OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

## GoogleTest
### GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
### For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

### OpenMP
find_package(OpenMP)

# Source files
add_subdirectory(src)

# Testing
add_subdirectory(tests)