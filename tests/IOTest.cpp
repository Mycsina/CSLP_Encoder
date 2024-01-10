#include <fstream>
#include <gtest/gtest.h>
#include <string>

#include "../src/io/BitStream.hpp"
#include "../src/io/Golomb.hpp"
#include "../src/visual/Video.hpp"
#include "../src/visual/YuvWriter.hpp"

auto golomb_dst = "../../tests/resource/golomb_test.bin";

TEST(IOTestSuite, BitStreamReadWriteTest) {
    auto *bs = new BitStream(golomb_dst, std::ios::out);
    bs->writeBit(1);
    bs->writeBits(0b111, 4);
    bs->writeBits(0b111, 4);
    delete bs;
    bs = new BitStream(golomb_dst, std::ios::in);
    ASSERT_EQ(bs->readBit(), 1);
    ASSERT_EQ(bs->readBits(4), 0b111);
    ASSERT_EQ(bs->readBits(4), 0b111);
    delete bs;
}

TEST(IOTestSuite, GolombReadWriteTest) {
    constexpr int m = 4;
    auto *g = new Golomb(golomb_dst, std::ios::out);
    g->encode(26, m);
    g->encode(-240, m);
    delete g;
    g = new Golomb(golomb_dst, std::ios::in);
    ASSERT_EQ(g->decode(), 26);
    ASSERT_EQ(g->decode(), -240);
    delete g;
    remove(golomb_dst);
}

TEST(IOTestSuite, YUV444WriteReadTest) {
    auto original_path = "../../tests/resource/ducks_take_off_444_720p50.y4m";
    Video yuv(original_path);
    auto saved_path = "../../tests/resource/ducks_take_off_444_720p50_out.y4m";
    YuvWriter yuv_writer(saved_path);
    yuv_writer.write_video(yuv);
    // compare files
    Video original(original_path);
    Video saved(saved_path);
    for (int i = 0; i < saved.get_reel().size(); i++) {
        Image original_image = original.get_frame(i).get_image();
        Image saved_image = saved.get_frame(i).get_image();
        ASSERT_TRUE(original_image == saved_image);
    }
}

TEST(IOTestSuite, YUV420WriteReadTest) {
    auto original_path = "../../tests/resource/akiyo_qcif.y4m";
    Video yuv(original_path);
    auto saved_path = "../../tests/resource/akiyo_qcif_out.y4m";
    YuvWriter yuv_writer(saved_path);
    yuv_writer.write_video(yuv);
    // compare files
    Video original(original_path);
    Video saved(saved_path);
    for (int i = 0; i < saved.get_reel().size(); i++) {
        Image original_image = original.get_frame(i).get_image();
        Image saved_image = saved.get_frame(i).get_image();
        ASSERT_TRUE(original_image == saved_image);
    }
}