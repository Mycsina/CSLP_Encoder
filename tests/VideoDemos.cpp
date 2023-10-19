#include "../src/visual/Video.cpp"
#include <gtest/gtest.h>

using namespace std;

auto vid_file = "../..tests/resource/video.mp4";
auto vid_yuv_file = "../..tests/resource/ducks_take_off_420_720p50.y4m";

TEST(VideoTestSuite, VideoTest) {
    Video vid;
    vid.load(vid_file);
    vid.play();
}

TEST(VideoTestSuite, Y4MVideoTest) {
    Video vid;
    vid.loadY4M(vid_yuv_file, YUV420);
    vid.play();
}

TEST(VideoTestSuite, Y4M2BGRVideoTest) {
    Video vid;
    vid.loadY4M(vid_yuv_file, YUV420);
    vid.convertTo(YUV, BGR);
    vid.play();
}
