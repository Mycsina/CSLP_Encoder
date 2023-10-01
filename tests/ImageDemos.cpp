#include "../src/visual/imageProcessing.cpp"
#include <gtest/gtest.h>

using namespace std;

auto demos_img = "../../tests/resource/tulips.ppm";

class ImageDemo : public ::testing::Test {
protected:
  Image im, im2, im3;
  void SetUp() override {
    im.load(demos_img);
    im2 = im.clone();
    im3 = im.clone();
  }
};

TEST_F(ImageDemo, YUV_BGR_Demo) {
  im.display_image();

  im2 = im.convert_BGR_YUV444();
  im2 = im2.convert_YUV_BGR();
  im2.display_image();

  im2 = im.convert_BGR_YUV422().convert_YUV_BGR();
  im2.display_image();

  im2 = im.convert_BGR_YUV420().convert_YUV_BGR();
  im2.display_image();
}

TEST_F(ImageDemo, ColorHist_Demo) {
  //// Histogram equalization demo ////
  // im.display_image();
  // equalize_hist(&im);
  ////////////////////////////////////
  im.display_image();
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

TEST_F(ImageDemo, WatermarkDemo) {
  string mark_file = "../../tests/resource/lena.ppm";
  Image mark;
  mark.load(mark_file);
  watermark(im, &mark, {0, 0},
            {mark.get_image_size()[1], mark.get_image_size()[0]}, 0.5);
  im.display_image();
}

TEST_F(ImageDemo, SubsamplingDemo) {
  im=im.convert_BGR_YUV444();
  // BGR2YUV(im);
  // im2.display_image();
  im.display_image();
  subsample(&im, YUV422);
  im.display_image();
  subsample(&im2, YUV420);
  im2.display_image();
}

TEST_F(ImageDemo, GaussianDemo){
    // 1  1  1
    // 1  1  1
    // 1  1  1
    Mat blur(3,3,CV_8UC1,Scalar(1));
    im.gaussian_blur(blur).display_image();
}
