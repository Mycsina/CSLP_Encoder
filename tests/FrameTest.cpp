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
        f1 = video.get_frame(0);
        f2 = video.get_frame(3);
        f3 = video.get_frame(6);
    }
};

TEST_F(FrameTest, IntraFrameTest) {
    f1.encode_JPEG_LS();
    int rows = f1.get_image().size().height;
    int cols = f1.get_image().size().width;
    Frame decoded = Frame::decode_JPEG_LS(f1.get_intra_encoding(), f1.get_image().get_color(), f1.get_image().get_chroma(), rows, cols);
    Image im1 = f1.get_image();
    Image im2 = decoded.get_image();
    ASSERT_TRUE(im1 == im2);
}

TEST_F(FrameTest, InterFrameTest) {
    f1.calculate_MV(&f2, 16, 7, false);
    Frame reconstruct = Frame::reconstruct_frame(f2, f1.get_motion_vectors(), 16);
    Image im1 = f1.get_image();
    Image im2 = reconstruct.get_image();
    ASSERT_TRUE(im1 == im2);
}

TEST_F(FrameTest, InterFrameTestFast) {
    auto comparator = new Block::SAD();
    comparator->threshold = 512;
    f1.set_block_diff(comparator);
    f1.calculate_MV(&f2, 16, 7, true);
    Frame reconstruct = Frame::reconstruct_frame(f2, f1.get_motion_vectors(), 16);
    Image im1 = f1.get_image();
    Image im2 = reconstruct.get_image();
    ASSERT_TRUE(im1 == im2);
}