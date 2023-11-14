#include "../src/io/BitStream.hpp"
#include "../src/io/Golomb.hpp"
#include <fstream>
#include <gtest/gtest.h>
#include <string>

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
    int m = 10;
    auto *g = new Golomb(golomb_dst, std::ios::out);
    g->encode(26, m);
    g->encode(-240, m);
    delete g;
    g = new Golomb(golomb_dst, std::ios::in);
    ASSERT_EQ(g->decode(), 26);
    ASSERT_EQ(g->decode(), -240);
}
