#include "../src/codec/LosslessIntra.hpp"
#include <gtest/gtest.h>

using namespace std;
using namespace cv;

class EncoderTest : public ::testing::Test {
protected:
    string normal_video = "../../tests/resource/video.mp4";
    string small_still = "../../tests/resource/akiyo_qcif.y4m";
    string small_moving = "../../tests/resource/coastguard_qcif.y4m";
    void SetUp() override {
    }
};

TEST_F(EncoderTest, lessIntraFrameEncoderTest) {
    auto *encoder = new LosslessIntraFrameEncoder(small_still.c_str(), "../../tests/resource/akiyo_encoded.y4m", 4);
    encoder->encode();
    delete encoder;
    auto *decoder = new LosslessIntraFrameEncoder("../../tests/resource/akiyo_encoded.y4m", "../../tests/resource/akiyo_decoded.y4m", 4);
    decoder->decode();
    delete decoder;
}