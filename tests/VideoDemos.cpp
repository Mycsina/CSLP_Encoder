#include "../src/visual/Image.hpp"
#include "../src/visual/Video.hpp"
#include <gtest/gtest.h>

using namespace std;

auto vid_file = "../../tests/resource/video.mp4";
auto vid_yuv_file = "../../tests/resource/akiyo_qcif.y4m";
auto yuv444_file = "../../tests/resource/ducks_take_off_444_720p50.y4m";

TEST(VideoTestSuite, VideoPlay) {
    Video vid;
    vid.load(vid_file);
    // ESC key
    vid.play(27);
}

TEST(VideoTestSuite, Y4MVideoTest) {
    Video vid;
    vid.load_y4m(vid_yuv_file);
    vid.play(27);
}

TEST(VideoTestSuite, Y4M2BGRVideoTest) {
    Video vid;
    vid.load_y4m(vid_yuv_file);
    vid.convert_to(YUV, BGR);
    vid.play(27);
}

TEST(VideoTestSuite, NewConstructorTest) {
    const Video vid1(vid_file);
    vid1.play(27);
    const Video vid2(vid_yuv_file);
    vid2.play(27);
}

TEST(VideoTestSuite, YUV444VideoTest) {
    const Video vid(yuv444_file);
    vid.play(27);
}