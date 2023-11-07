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
        auto video = Video();
        video.load(smallStill);
        Frame f1 = video.getFrame(0);
        Frame f2 = video.getFrame(1);
        Frame f3 = video.getFrame(2);
    }
};

TEST_F(FrameDemo, FrameMotionVectorDemo) {
    auto vid = Video();
    vid.load(frameTestVideo);
    // vid.loadY4M(frameTestVideo, YUV420);
    // vid.convertTo(YUV, BGR);
    f1.display_frame_original();
    f3.display_frame_original();
    int block_size = 16;
    f3.calculate_MV(block_size, &f1, 7, false);
    int i = 0;
    int j = 0;
    Mat res = Mat::zeros(f3.getImage().size()[0], f3.getImage().size()[1], CV_8UC3);
    for (const auto &v: f3.getMotionVectors()) {
        set_slice(res, v.residual, j, i);
        arrowedLine(res, Point(i + block_size / 2, j + block_size / 2), Point(i + v.x + block_size / 2, j + v.y + block_size / 2), Scalar(0, 0, 255), 1, 8, 0);
        i += block_size;
        if (i >= res.cols) {
            i = 0;
            j += block_size;
        }
    };
    imshow("res", res);
    waitKey(0);
}