#include "../src/visual/ImageProcessing.hpp"
#include <gtest/gtest.h>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

auto imageProcImg = "../../tests/resource/tulips.ppm";

class ImageTest : public ::testing::Test {
protected:
    Image img, im2, im3;
    void SetUp() override {
        img.load(imageProcImg);
        im2 = img.clone();
        im3 = img.clone();
    }
};

TEST_F(ImageTest, YUVEquivalenceTest) {
    Image im(imageProcImg);
    Image img = convert_BGR_YUV444(im);
    // img.display_image();
    cvtColor(*im2._get_image_mat(), *im2._get_image_mat(), COLOR_BGR2YUV);
    // im2.display_image();
    int rand_row = rand() % img.size()[0];
    int rand_col = rand() % img.size()[1];
    Vec3b color = img._get_image_mat()->at<Vec3b>(rand_row, rand_col);
    Vec3b color2 = im2._get_image_mat()->at<Vec3b>(rand_row, rand_col);
    // Absolute difference between the two colors should be less than 3
    int diff = abs((color[0] + color[1] + color[2]) -
                   (color2[0] + color2[1] + color2[2]));
    EXPECT_TRUE(diff < 3);
}

TEST_F(ImageTest, BGREquivalenceTest) {
    cvtColor(*img._get_image_mat(), *img._get_image_mat(), COLOR_BGR2YUV);
    // img.display_image();
    cvtColor(*im2._get_image_mat(), *im2._get_image_mat(), COLOR_BGR2YUV);
    // im2.display_image();
    cvtColor(*im2._get_image_mat(), *im2._get_image_mat(), COLOR_YUV2BGR);
    // img.display_image();
    img = convert_YUV_BGR(img);
    // img.display_image();
    int rand_row = rand() % img.size()[0];
    int rand_col = rand() % img.size()[1];
    Vec3b color = img._get_image_mat()->at<Vec3b>(rand_row, rand_col);
    Vec3b color2 = im2._get_image_mat()->at<Vec3b>(rand_row, rand_col);
    // Absolute difference between the two colors should be less than 3
    int diff = abs((color[0] + color[1] + color[2]) -
                   (color2[0] + color2[1] + color2[2]));
    EXPECT_TRUE(diff < 3);
}

TEST_F(ImageTest, GRAYEquivalenceTest) {
    cvtColor(*img._get_image_mat(), *img._get_image_mat(), COLOR_BGR2GRAY);
    // img.display_image();
    im2 = convert_BGR_GRAY(im2);
    // im2.display_image();
    int rand_row = rand() % img.size()[0];
    int rand_col = rand() % img.size()[1];
    uchar color = img._get_image_mat()->at<uchar>(rand_row, rand_col);
    uchar color2 = im2._get_image_mat()->at<uchar>(rand_row, rand_col);
    // Absolute difference between the two colors should be less than 3
    int diff = abs(color - color2);
    EXPECT_TRUE(diff < 3);
}

TEST_F(ImageTest, ConversionIdempotenceTest) {
    // BGR->YUV->BGR
    cvtColor(*img._get_image_mat(), *img._get_image_mat(), COLOR_BGR2YUV);
    im2 = convert_BGR_YUV444(im2);
    // img.display_image();
    // im2.display_image();
    cvtColor(*img._get_image_mat(), *img._get_image_mat(), COLOR_YUV2BGR);
    im2 = convert_YUV_BGR(im2);
    // img.display_image();
    // im2.display_image();
    int rand_row = rand() % img.size()[0];
    int rand_col = rand() % img.size()[1];
    Vec3b color = img._get_image_mat()->at<Vec3b>(rand_row, rand_col);
    Vec3b color2 = im2._get_image_mat()->at<Vec3b>(rand_row, rand_col);
    // Absolute difference between the two colors should be less than 3
    int diff = abs((color[0] + color[1] + color[2]) -
                   (color2[0] + color2[1] + color2[2]));
    EXPECT_TRUE(diff < 3);
}

TEST_F(ImageTest, HistogramEqualiz) {
    // Test if our self implemented histogram equalization works
    img = convert_YUV_BGR(img);
    img = convert_BGR_GRAY(img);
    im2 = convert_YUV_BGR(im2);
    im2 = convert_BGR_GRAY(im2);
    equalize_hist(img);
    equalizeHist(*im2._get_image_mat(), *im2._get_image_mat());
    int rand_row = rand() % img.size()[0];
    int rand_col = rand() % img.size()[1];
    Vec3b color = img._get_image_mat()->at<Vec3b>(rand_row, rand_col);
    Vec3b color2 = im2._get_image_mat()->at<Vec3b>(rand_row, rand_col);
    // Absolute difference between the two colors should be less than 3
    int diff = abs((color[0] + color[1] + color[2]) -
                   (color2[0] + color2[1] + color2[2]));
    EXPECT_TRUE(diff < 3);
}