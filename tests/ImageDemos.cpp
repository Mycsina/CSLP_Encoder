#include "../src/image.cpp"
#include "../src/imageProcessing.hpp"
#include <gtest/gtest.h>

using namespace std;

// auto img_file = "../../tests/resource/img.png";
auto img_file = "../../tests/resource/tulips.ppm";

TEST(ImageDemos, YUV_BGR_Demo) {
  image im1;
  im1.load(img_file);
  im1.display_image();

  image im2;
  im2 = im1.convert_BGR_YUV444();
  im2 = im2.convert_YUV_BGR();
  im2.display_image();

  im2 = im1.convert_BGR_YUV422().convert_YUV_BGR();
  im2.display_image();

  im2 = im1.convert_BGR_YUV420().convert_YUV_BGR();
  im2.display_image();
}

TEST(ImageDemos, ColorHist_Demo) {
  image im;
  im.load(img_file);
  // im.display_image();
  vector<Mat> hists = im.color_histograms(256, true, 1024, 800);
  // NOTE I found that if you try to allocate a new array with same shape and
  // type, it will return the previous array, so it's needed to assure that it
  // is filled with zeroes
  Mat carry = Mat::zeros(hists[0].rows, hists[0].cols, CV_8UC3);
  for (auto &hist : hists) {
    imshow("Histogram", hist);
    waitKey(0);
    for (int i = 0; i < hist.rows; i++) {
      for (int j = 0; j < hist.cols; j++) {
        carry.at<Vec3b>(i, j) += hist.at<Vec3b>(i, j);
      }
    }
  }
  imshow("Histogram", carry);
  waitKey(0);
}

TEST(ImageDemos, WatermarkDemo) {
  string mark_file = "../../tests/resource/lena.ppm";
  image im;
  im.load(img_file);
  image mark;
  mark.load(mark_file);
  watermark(im, &mark, {0, 0},
            {mark.get_image_size()[1], mark.get_image_size()[0]}, 0.5);
  im.display_image();
}

TEST(ImageDemos, SubsamplingDemo) {
  image im;
  im.load(img_file);
  cvtColor(*im._get_image_mat(), *im._get_image_mat(), COLOR_BGR2YUV);
  // BGR2YUV(im);
  image im2;
  im2 = im.clone();
  // im2.display_image();
  im.display_image();
  subsample(im, YUV422);
  im.display_image();
  subsample(im2, YUV420);
  im2.display_image();
}
