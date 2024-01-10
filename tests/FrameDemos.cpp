#include "../src/codec/Frame.hpp"
#include "../src/visual/Video.hpp"
#include <gtest/gtest.h>

using namespace std;
using namespace cv;

auto frameTestVideo = "../../tests/resource/video.mp4";
auto smallStill = "../../tests/resource/akiyo_qcif.y4m";
auto smallMoving = "../../tests/resource/coastguard_qcif.y4m";
auto test_video = "../../tests/resource/ducks_take_off_444_720p50.y4m";

class FrameDemo : public ::testing::Test {
protected:
    Frame f1, f2, f3;
    void SetUp() override {
        auto vid = Video();
        vid.load_y4m(test_video);
        vid.convert_to(YUV, BGR);
        f1 = vid.get_frame(0);
        f2 = vid.get_frame(3);
        f3 = vid.get_frame(6);
    }
};

TEST_F(FrameDemo, FrameMotionVectorDemo) {
    f1.show();
    f3.show();
    constexpr int block_size = 16;
    f3.calculate_MV(f1, block_size, 10, false);
    f3.visualize_MV(f1, block_size);
}

TEST_F(FrameDemo, FrameReconstructionDemo) {
    f1.show();
    f3.show();
    constexpr int block_size = 16;
    f3.calculate_MV(f1, block_size, 10, false);
    Frame reconstruct = Frame::reconstruct_frame(f1, f3.get_motion_vectors(), block_size);
    reconstruct.show();
}