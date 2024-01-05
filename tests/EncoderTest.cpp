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
    string test_video = "../../tests/resource/ducks_take_off_444_720p50.y4m";
    void SetUp() override {
    }
};

TEST_F(EncoderTest, HybridTest) {
    constexpr int m = 3;
    const char *file = test_video.c_str();
    auto encoder = LosslessHybridEncoder(file, "../../tests/resource/encoded", m, 16, 5);
    encoder.encode();
    auto decoder = LosslessHybridEncoder("../../tests/resource/encoded", "../../tests/resource/decoded", m, 16, 5);
    decoder.decode();
    const auto video_frames = Video(file).generate_frames();
    for (int i = 0; i < video_frames.size(); i++) {
        Image im1 = video_frames[i]->get_image();
        Image im2 = decoder.frames[i].get_image();
        ASSERT_TRUE(im1 == im2);
    }
}


TEST_F(EncoderTest, IntraTest) {
    const char *file = test_video.c_str();
    auto encoder = LosslessIntraEncoder(file, "../../tests/resource/encoded");
    encoder.encode();
    auto decoder = LosslessIntraEncoder("../../tests/resource/encoded", "../../tests/resource/decoded");
    decoder.decode();
    const auto video_frames = Video(file).generate_frames();
    for (int i = 0; i < video_frames.size(); i++) {
        Image im1 = video_frames[i]->get_image();
        Image im2 = decoder.frames[i].get_image();
        ASSERT_TRUE(im1 == im2);
    }
}