#include "../src/io/BitStream.cpp"
#include "../src/io/Golomb.cpp"
#include <gtest/gtest.h>
#include <stdio.h>
#include <fstream>
#include <string>

auto golomb_dst="../../tests/resource/golomb_test.bin";

TEST(IOTestSuite, BitStreamReadWriteTest){
    BitStream* bs= new BitStream(golomb_dst, std::ios::out);
    bs->writeBit(1);
    bs->writeBits(0b111,4);
    bs->writeBits(0b111,4);
    delete bs;
    bs=new BitStream(golomb_dst, std::ios::in);
    ASSERT_EQ(bs->readBit(),1);
    ASSERT_EQ(bs->readBits(4),0b111);
    ASSERT_EQ(bs->readBits(4),0b111);
}

//TODO: check wrong value
TEST(IOTestSuite, GolombReadWriteTest){
    int m=10;
    Golomb *g=new Golomb(golomb_dst);
    g->encode(25,m);
    delete g;
    g=new Golomb(golomb_dst);
    std::cout << g->decode() << std::endl;
}
