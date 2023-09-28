#include <gtest/gtest.h>
#include "../src/video.cpp"

using namespace std;

auto vid_file = "../../tests/resource/video.mp4";

TEST(VideoTestSuite, VideoTest) {
    video vid;
    vid.load(vid_file);
}