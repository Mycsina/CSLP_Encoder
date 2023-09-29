#include <gtest/gtest.h>
#include "../src/video.cpp"

using namespace std;

auto vid_file = "../../tests/resource/video.mp4";
auto vid_yuv_file = "../../tests/resource/ducks_take_off_420_720p50.y4m";

TEST(VideoTestSuite, VideoTest){
    video vid;
    vid.load(vid_file);
    vid.play();
}

TEST(VideoTestSuite, Y4MVideoTest){
    video vid;
    vid.loadY4M(vid_yuv_file,YUV420);
    vid.play();
}