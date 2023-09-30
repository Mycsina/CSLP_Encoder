#include "../src/imageProcessing.cpp"
#include <gtest/gtest.h>
#include <opencv2/highgui.hpp>

using namespace std;

auto img_name = "../../tests/resource/tulips.ppm";

TEST(ImageProcessingSuite, YUVEquivalenceTest) {
  image im;
  im.load(img_name);
  image im2;
  im2.load(img_name);
  // im.display_image();
  // BGR2YUV(im);
  im.convert_BGR_YUV444();
  im.display_image();
  cvtColor(*im2._get_image_mat(), *im2._get_image_mat(), COLOR_BGR2YUV);
  im2.display_image();
  int rand_row = rand() % im.get_image_size()[0];
  int rand_col = rand() % im.get_image_size()[1];
  Vec3b color = im._get_image_mat()->at<Vec3b>(rand_row, rand_col);
  Vec3b color2 = im2._get_image_mat()->at<Vec3b>(rand_row, rand_col);
  // Absolute difference between the two colors should be less than 3
  int diff = abs((color[0] + color[1] + color[2]) -
                 (color2[0] + color2[1] + color2[2]));
  EXPECT_TRUE(diff < 3);
}

TEST(ImageProcessingSuite, BGREquivalenceTest) {
  image im;
  im.load(img_name);
  cvtColor(*im._get_image_mat(), *im._get_image_mat(), COLOR_BGR2YUV);
  // im.display_image();
  image im2;
  im2.load(img_name);
  cvtColor(*im2._get_image_mat(), *im2._get_image_mat(), COLOR_BGR2YUV);
  // im2.display_image();
  cvtColor(*im2._get_image_mat(), *im2._get_image_mat(), COLOR_YUV2BGR);
  // im.display_image();
  YUV2BGR(&im);
  // im.display_image();
  int rand_row = rand() % im.get_image_size()[0];
  int rand_col = rand() % im.get_image_size()[1];
  Vec3b color = im._get_image_mat()->at<Vec3b>(rand_row, rand_col);
  Vec3b color2 = im2._get_image_mat()->at<Vec3b>(rand_row, rand_col);
  // Absolute difference between the two colors should be less than 3
  int diff = abs((color[0] + color[1] + color[2]) -
                 (color2[0] + color2[1] + color2[2]));
  EXPECT_TRUE(diff < 3);
}

TEST(ImageProcessingSuit, GRAYEquivalenceTest) {
  image im;
  im.load(img_name);
  cvtColor(*im._get_image_mat(), *im._get_image_mat(), COLOR_BGR2GRAY);
  im.display_image();
  image im2;
  im2.load(img_name);
  BGR2GRAY(&im2);
  im2.display_image();
  int rand_row = rand() % im.get_image_size()[0];
  int rand_col = rand() % im.get_image_size()[1];
  uchar color = im._get_image_mat()->at<uchar>(rand_row, rand_col);
  uchar color2 = im2._get_image_mat()->at<uchar>(rand_row, rand_col);
  // Absolute difference between the two colors should be less than 3
  int diff = abs(color - color2);
  EXPECT_TRUE(diff < 3);
}

TEST(ImageProcessingSuite, ConversionIdempotenceTest) {
  image im;
  im.load(img_name);
  image im2;
  im2.load(img_name);
  // TODO make it work with our conversion functions
  cvtColor(*im._get_image_mat(), *im._get_image_mat(), COLOR_BGR2YUV);
  // BGR2YUV(im);
  // im.display_image();
  cvtColor(*im._get_image_mat(), *im._get_image_mat(), COLOR_YUV2BGR);
  // YUV2BGR(im);
  // im.display_image();
  int rand_row = rand() % im.get_image_size()[0];
  int rand_col = rand() % im.get_image_size()[1];
  Vec3b color = im._get_image_mat()->at<Vec3b>(rand_row, rand_col);
  Vec3b color2 = im2._get_image_mat()->at<Vec3b>(rand_row, rand_col);
  // Absolute difference between the two colors should be less than 3
  int diff = abs((color[0] + color[1] + color[2]) -
                 (color2[0] + color2[1] + color2[2]));
  EXPECT_TRUE(diff < 3);
}
