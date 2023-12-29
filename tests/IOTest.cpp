#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
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
    remove(golomb_dst);
}

TEST(IOTestSuite, YUV444WriteReadTest) {
    auto yuv444_file = "../../tests/resource/ducks_take_off_444_720p50.y4m";
    Video yuv444(yuv444_file);
    auto yuv444_out = "../../tests/resource/ducks_take_off_444_720p50_out.y4m";
    YuvWriter yuv444_writer(yuv444_out);
    yuv444_writer.write_video(yuv444);
    // compare files
    std::ifstream yuv444_ifs(yuv444_file, std::ios::binary);
    std::ifstream yuv444_out_ifs(yuv444_out, std::ios::binary);
    while (yuv444_ifs.good() && yuv444_out_ifs.good()) {
        ASSERT_EQ(yuv444_ifs.get(), yuv444_out_ifs.get());
    }
}

TEST(IOTestSuite, YUV420WriteReadTest) {
    auto yuv420_file = "../../tests/resource/akiyo_qcif.y4m";
    Video yuv420(yuv420_file);
    auto yuv420_out = "../../tests/resource/akiyo_qcif_out.y4m";
    YuvWriter yuv420_writer(yuv420_out);
    yuv420_writer.write_video(yuv420);
    // compare files
    std::ifstream yuv420_ifs(yuv420_file, std::ios::binary);
    std::ifstream yuv420_out_ifs(yuv420_out, std::ios::binary);
    while (yuv420_ifs.good() && yuv420_out_ifs.good()) {
        std::cout << yuv420_ifs.tellg() << std::endl;
        ASSERT_EQ(yuv420_ifs.get(), yuv420_out_ifs.get());
    }
}