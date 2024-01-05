#include "../src/codec/encoders/LosslessHybrid.hpp"
#include "../src/codec/encoders/LosslessIntra.hpp"
#include "../src/codec/encoders/LossyHybrid.hpp"
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
    const char *file = small_still;
    auto encoder = LosslessIntraEncoder(file, "../../tests/resource/encoded");
    encoder.encode();
    auto decoder = LosslessIntraEncoder("../../tests/resource/encoded", "../../tests/resource/decoded");
    decoder.decode();
    for (auto &frame: decoder.frames) {
        Image im2 = frame.get_image();
        im2.show(true);
    }
}

TEST_F(EncoderDemo, HybridDemo) {
    constexpr int m = 2;
    const char *file = small_still;
    LosslessHybridEncoder encoder(file, "encoded", m, 16, 5);
    encoder.encode();
    LosslessHybridEncoder decoder("encoded", "decoded", m, 16, 5);
    decoder.decode();
    for (auto &frame: decoder.frames) {
        Image im2 = frame.get_image();
        im2.show(true);
    }
}

TEST_F(EncoderDemo, LossyHybridDemo) {
    constexpr int m = 2;
    const char *file = small_still;
    LossyHybridEncoder encoder(file, "encoded", m, 16, 5, 255, 255, 255);
    encoder.encode();
    LossyHybridEncoder decoder("encoded", "decoded", m, 16, 5, 255, 255, 255);
    decoder.decode();
    for (auto &frame: decoder.frames) {
        Image im2 = frame.get_image();
        im2.show(true);
    }
}