#include "../src/codec/LosslessIntra.hpp"
#include <gtest/gtest.h>
#include <iostream>

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
    string file = small_still;
    string encoded = file + "_encoded";
    string decoded = file + "_decoded";
    int golomb_m = 2;
    auto *encoder = new LosslessIntraFrameEncoder(file.c_str(), encoded.c_str(), golomb_m);
    encoder->encode();
    delete encoder;
    auto *decoder = new LosslessIntraFrameEncoder(encoded.c_str(), decoded.c_str(), golomb_m);
    decoder->decode();
    delete decoder;
}