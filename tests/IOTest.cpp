#include "../src/io/BitStream.cpp"
#include "../src/io/Golomb.cpp"
#include <gtest/gtest.h>
#include <stdio.h>

auto golomb_dst="../../tests/resource/golomb_test.bin"

TEST(IOTestSuite, GolombReadWriteTest){
    remove(golomb_dst);
    int m=10;
    Golomb g(golomb_dst);
    g.encode(69,m);
    Golomb g2(golomb_dst);
    ASSERT_EQ(g.decode(),69);
    remove(golomb_dst);
};
