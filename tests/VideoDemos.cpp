#include "../src/visual/Video.cpp"
#include <gtest/gtest.h>

using namespace std;

auto vid_file = "../../tests/resource/video.mp4";
auto vid_yuv_file = "../../tests/resource/akiyo_qcif.y4m";

TEST(VideoTestSuite, VideoPlay) {
    Video vid;
    vid.load(vid_file);
    // ESC key
    vid.play(27);
}

TEST(VideoTestSuite, Y4MVideoTest) {
    Video vid;
    vid.loadY4M(vid_yuv_file, YUV420);
    vid.play(27);
}

TEST(VideoTestSuite, Y4M2BGRVideoTest) {
    Video vid;
    vid.loadY4M(vid_yuv_file, YUV420);
    vid.convertTo(YUV, BGR);
    vid.play(27);
}
