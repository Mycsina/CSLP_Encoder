#include "../src/codec/Frame.hpp"
#include "../src/visual/Video.hpp"
#include <gtest/gtest.h>

using namespace std;

auto frameTestVideo = "../../tests/resource/video.mp4";
auto smallFrameTestVideo = "../../tests/resource/akiyo_qcif.y4m";

class FrameTest : public ::testing::Test {
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