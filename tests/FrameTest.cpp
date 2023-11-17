#include "../src/codec/Frame.hpp"
#include "../src/visual/Image.hpp"
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
        f1 = video.getFrame(0);
        f2 = video.getFrame(3);
        f3 = video.getFrame(6);
    }
};

TEST_F(FrameTest, IntraFrameTest) {
    f1.encode_JPEG_LS();
    int rows = f1.getImage().size()[0];
    int cols = f1.getImage().size()[1];
    Frame decoded = Frame::decode_JPEG_LS(f1.getIntraEncoding(), f1.getImage().getColor(), f1.getImage().getChroma(), rows, cols);
    Image im1 = f1.getImage();
    Image im2 = decoded.getImage();
    ASSERT_TRUE(im1 == im2);
}

TEST_F(FrameTest, InterFrameTest) {
    f1.calculate_MV(&f2, 16, 7, false);
    Frame reconstruct = Frame::reconstruct_frame(&f2, f1.getMotionVectors(), 16);
    Image im1 = f1.getImage();
    Image im2 = reconstruct.getImage();
    im1.show();
    im2.show();
    ASSERT_TRUE(im1 == im2);
}