#include "../src/visual/Video.cpp"


#include "../src/codec/encoders/lossy/LossyHybrid.hpp"

#include <gtest/gtest.h>

using namespace std;

auto vid_file = "../../tests/resource/video.mp4";
auto small_still = "../../tests/resource/akiyo_qcif.y4m";
auto vid_yuv_file = "../../tests/resource/ducks_take_off_444_720p50.y4m";

TEST(VideoTest, CompareEqual) {
    const auto video = vid_yuv_file;
    const Video original(video);
    Video copy(video);
    const auto psnr = original.compare(copy);
    try {
        ASSERT_TRUE(psnr == INFINITY);
    } catch (const std::exception &e) {
        std::cout << "Check that -Ofast is not enabled. It breaks the test." << std::endl;
        std::cout << e.what() << std::endl;
    }
}

TEST(VideoTest, CompareDifferent) {
    auto video = vid_yuv_file;
    auto encoded = "../../tests/resource/encoded";
    auto decoded = "../../tests/resource/decoded";
    Video original(video);
    LossyHybridEncoder encoder(video, encoded, 4, 16, 5, 128, 128, 128);
    encoder.encode();
    LossyHybridEncoder decoder(encoded, decoded);
    decoder.decode();
    Video copy(decoded);
    auto psnr = original.compare(copy);
    ASSERT_TRUE(psnr < INFINITY);
}