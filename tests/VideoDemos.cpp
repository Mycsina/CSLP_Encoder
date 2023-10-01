#include "../src/visual/Image.cpp"
#include "../src/visual/video.cpp"
#include <gtest/gtest.h>

using namespace std;

// auto img_file = "../../tests/resource/img.png";
auto vid_file = "../../tests/resource/video.mp4";
auto vid_yuv_file = "../../tests/resource/ducks_take_off_420_720p50.y4m";

TEST(VideoDemos, VideoTest) {
  video vid;
  vid.load(vid_file);
  vid.play();
}

TEST(VideoDemos, Y4MVideoTest) {
  video vid;
  vid.loadY4M(vid_yuv_file, YUV420);
  vid.play();
}

TEST(VideoTestSuite, Y4M2BGRVideoTest)
{
    video vid;
    vid.loadY4M(vid_yuv_file, YUV420);
    vid.convertTo(YUV, BGR);
    vid.play();
}