#include "../src/codec/Frame.hpp"
#include "../src/visual/Video.hpp"
#include <gtest/gtest.h>

using namespace std;
using namespace cv;

auto frameTestVideo = "../../tests/resource/video.mp4";
auto smallStill = "../../tests/resource/akiyo_qcif.y4m";
auto smallMoving = "../../tests/resource/coastguard_qcif.y4m";

class FrameDemo : public ::testing::Test {
protected:
    Frame f1, f2, f3;
    void SetUp() override {
        auto vid = Video();
        vid.loadY4M(smallMoving, YUV420);
        vid.convertTo(YUV, BGR);
        f1 = vid.getFrame(0);
        f2 = vid.getFrame(3);
        f3 = vid.getFrame(6);
    }
};

TEST_F(FrameDemo, FrameMotionVectorDemo) {
    f1.show();
    f3.show();
    int block_size = 16;
    f3.calculate_MV(&f1, block_size, 7, false);
    f3.visualize_MV(&f1, block_size);
}

TEST_F(FrameDemo, FrameReconstructionDemo) {
    f1.show();
    f3.show();
    int block_size = 16;
    f3.calculate_MV(&f1, block_size, 7, false);
    Frame reconstruct = Frame::reconstruct_frame(&f1, f3.getMotionVectors(), block_size);
    reconstruct.show();
}