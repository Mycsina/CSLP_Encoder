#include "../src/codec/encoders/lossless/LosslessHybrid.hpp"
#include "../src/codec/encoders/lossless/LosslessIntra.hpp"
#include "../src/codec/encoders/lossy/LossyHybrid.hpp"
#include "../src/codec/encoders/lossy/LossyIntra.hpp"
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
    const char *test_video = "../../tests/resource/ducks_take_off_444_720p50.y4m";
    void SetUp() override {
    }
};

TEST_F(EncoderDemo, IntraDemo) {
    const char *file = small_moving;
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
    const char *file = small_moving;
    LosslessHybridEncoder encoder(file, "encoded", m, 16, 5);
    encoder.encode();
    LosslessHybridEncoder decoder("encoded");
    decoder.decode();
    for (auto &frame: decoder.frames) {
        Image im2 = frame.get_image();
        im2.show(true);
    }
}

TEST_F(EncoderDemo, LossyIntraDemo) {
    constexpr int m = 2;
    const char *file = test_video;
    LossyIntraEncoder encoder(file, "../../tests/resource/encoded", m, 64, 32, 32);
    encoder.encode();
    LossyIntraEncoder decoder("../../tests/resource/encoded");
    decoder.decode();
    Video vid(decoder.frames);
    vid.convert_to(YUV, BGR);
    vid.play();
}

TEST_F(EncoderDemo, LossyHybridDemo) {
    constexpr int m = 2;
    const char *file = test_video;
    LossyHybridEncoder encoder(file, "../../tests/resource/encoded", m, 16, 5, 64, 32, 32);
    encoder.encode();
    LossyHybridEncoder decoder("../../tests/resource/encoded");
    decoder.decode();
    Video vid(decoder.frames);
    vid.convert_to(YUV, BGR);
    vid.play();
}