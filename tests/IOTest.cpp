#include "../src/io/BitStream.cpp"
#include "../src/io/Golomb.cpp"
#include <gtest/gtest.h>
#include <stdio.h>

auto golomb_dst="../../tests/resource/golomb_test.bin";

//TODO: check wrong value
TEST(IOTestSuite, GolombReadWriteTest){
    remove(golomb_dst);
    int m=10;
    Golomb g(golomb_dst);
    g.encode(25,m);
    g.reset();
    ASSERT_EQ(g.decode(),25);
    remove(golomb_dst);
}
