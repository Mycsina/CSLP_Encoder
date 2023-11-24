#include "../src/visual/Image.hpp"
#include "../src/visual/Video.hpp"
#include <gtest/gtest.h>

using namespace std;

auto vid_file = "../../tests/resource/video.mp4";
auto vid_yuv_file = "../../tests/resource/akiyo_qcif.y4m";
auto interframe_dst = "../../tests/resource/interframe_test.bin";

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

TEST(VideoTestSuite, NewConstructorTest) {
    Video vid1(vid_file);
    vid1.play(27);
    Video vid2(vid_yuv_file);
    vid2.play(27);
}

TEST(VideoTestSuite, InterframeDemoSlow) {
    Video vid1(vid_yuv_file);
    vid1.encode_hybrid(interframe_dst,2,5,10,8);
    Video vid2=Video::decode_hybrid(interframe_dst);
    vid2.play(27);
}