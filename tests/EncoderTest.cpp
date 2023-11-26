#include "../src/codec/encoders/LosslessInter.hpp"
#include "../src/codec/encoders/LosslessIntra.hpp"
#include "../src/codec/encoders/LosslessHybrid.hpp"
#include "../src/visual/Video.hpp"
#include <gtest/gtest.h>

using namespace std;

class EncoderTest : public ::testing::Test {
protected:
    string normal_video = "../../tests/resource/video.mp4";
    string small_still = "../../tests/resource/akiyo_qcif.y4m";
    string small_moving = "../../tests/resource/coastguard_qcif.y4m";
    void SetUp() override {
    }
};

TEST_F(EncoderTest, HybridTest) {
    const int m = 2;
    const char *file = small_still.c_str();
    LosslessHybridEncoder encoder = LosslessHybridEncoder(file, "encoded", m, 16, 5);
    encoder.encode();
    LosslessHybridEncoder decoder = LosslessHybridEncoder("encoded", "decoded", m, 16, 5);
    decoder.decode();
    const auto video_frames = Video(file).generate_frames();
    for (int i = 0; i < video_frames.size(); i++) {
        Image im1 = video_frames[i]->get_image();
        Image im2 = decoder.frames[i].get_image();
        ASSERT_TRUE(im1 == im2);
    }
}


TEST_F(EncoderTest, IntraTest) {
    const char *file = small_still.c_str();
    auto *encoder = new LosslessIntraEncoder(file, "../../tests/resource/encoded");
    encoder->encode();
    delete encoder;
    auto *decoder = new LosslessIntraEncoder("../../tests/resource/encoded", "decoded");
    decoder->decode();
    const auto video_frames = Video(file).generate_frames();
    for (int i = 0; i < video_frames.size() - 1; i++) {
        Image im1 = video_frames[i]->get_image();
        Image im2 = decoder->frames[i].get_image();
        ASSERT_TRUE(im1 == im2);
    }
}