#include "../src/codec/Frame.hpp"
#include "../src/visual/Video.hpp"
#include <gtest/gtest.h>

using namespace std;

auto frameTestVideo = "../../tests/resource/video.mp4";
auto smallFrameTestVideo = "../../tests/resource/akiyo_qcif.y4m";

class FrameDemo : public ::testing::Test {
protected:
    Frame f1, f2, f3;
    void SetUp() override {
        auto video = Video();
        video.load(frameTestVideo);
        Image im1 = video.getFrame(0);
        f1 = Frame(im1);
        Image im2 = video.getFrame(1);
        f2 = Frame(im2);
        Image im3 = video.getFrame(2);
        f3 = Frame(im3);
        f3.setPrevious(&f2);
        f2.setPrevious(&f1);
    }
};

TEST_F(FrameDemo, FrameMotionVectorDemo) {
    auto vid = Video();
    vid.loadY4M(smallFrameTestVideo, YUV420);
    vid.convertTo(YUV, BGR);
    auto f1 = Frame(vid.getFrame(0));
    auto f2 = Frame(vid.getFrame(3));
    auto f3 = Frame(vid.getFrame(6));
    f3.setPrevious(&f2);
    f2.setPrevious(&f1);
//    f1.display_frame_original();
//    f3.display_frame_original();
    f3.match_all_blocks(16, 2, 7, false);
    for (auto v: f3.getMotionVectors()) {
        cout << v.x << " " << v.y << endl;
    }
}