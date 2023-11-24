project(CSLP_Project)

include_directories(${gtest_SOURCE_DIR_}/include ${gtest_SOURCE_DIR_})
add_definitions(-D_DEMO)
add_executable(tests_run
        ImageTest.cpp
        VideoTest.cpp
        ImageProcTest.cpp
        IOTest.cpp
        FrameTest.cpp
        EncoderTest.cpp)

add_executable(Demos
        VideoDemos.cpp
        ImageDemos.cpp
        FrameDemos.cpp
        EncoderDemos.cpp)


message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

target_link_libraries(
        tests_run
        gtest_main
        Visual
        Codec
        BitStream
)

target_link_libraries(
        Demos
        gtest_main
        Visual
        Codec
        BitStream
)