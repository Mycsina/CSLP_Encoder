#include <gtest/gtest.h>
#include "../src/Image.cpp"

TEST(ImageTestSuite, ImageLoadTest){
    Image image;
    EXPECT_TRUE(image.load("/mnt/Projects/Uni/CSLP/Project/tests/resource/img.png"));
}