#include "../src/codec/encoders/LosslessHybrid.hpp"
#include "../src/codec/encoders/LosslessIntra.hpp"
#include "../src/codec/encoders/DCTEncoder.hpp"
#include "../src/visual/Video.hpp"
#include <gtest/gtest.h>
#include <iostream>

using namespace std;
using namespace cv;

class EncoderDemo : public ::testing::Test {
protected:
    const char *normal_video = "../../tests/resource/video.mp4";
    const char *small_still = "../../tests/resource/akiyo_qcif.y4m";
    const char *small_moving = "../../tests/resource/coastguard_qcif.y4m";
    void SetUp() override {
    }
};

TEST_F(EncoderDemo, IntraDemo) {
    const char *file = small_moving;
    auto *encoder = new LosslessIntraEncoder(file, "../../tests/resource/encoded");
    encoder->encode();
    delete encoder;
    auto *decoder = new LosslessIntraEncoder("../../tests/resource/encoded", "decoded");
    decoder->decode();
    for (auto &frame: decoder->frames) {
        Image im2 = frame.get_image();
        im2.show();
    }
}

TEST_F(EncoderDemo, HybridDemo) {
    const int m = 2;
    const char *file = small_still;
    LosslessHybridEncoder encoder(file, "../../tests/resource/encoded", m, 16, 5);
    encoder.encode();
    LosslessHybridEncoder decoder("../../tests/resource/encoded", "../../tests/resource/decoded", m, 16, 5);
    decoder.decode();
    for (auto &frame: decoder.frames) {
        Image im2 = frame.get_image();
        im2.show();
    }
}
TEST_F(EncoderDemo, DCTEncode){
    const char *file = small_still;
    auto *encoder = new DCTEncoder(file,"../../tests/resource/encoded",2);
    encoder->encode();
    delete encoder;
}