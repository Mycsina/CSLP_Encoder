project(tests)

enable_testing()

include_directories(${gtest_SOURCE_DIR_}/include ${gtest_SOURCE_DIR_})
set(SOURCE_FILES ImageTest.cpp VideoTest.cpp)
add_executable(tests_run ${SOURCE_FILES})

message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

target_link_libraries(
        tests_run
        gtest_main
        ${OpenCV_LIBS}
)
