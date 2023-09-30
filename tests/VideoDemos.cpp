#include "../src/image.cpp"
#include "../src/video.cpp"
#include <gtest/gtest.h>

using namespace std;

// auto img_file = "../../tests/resource/img.png";
auto img_file = "../../tests/resource/tulips.ppm";
auto vid_file = "../../tests/resource/video.mp4";
auto vid_yuv_file = "../../tests/resource/ducks_take_off_420_720p50.y4m";

TEST(VideoDemos, VideoTest) {
  video vid;
  vid.load(vid_file);
  vid.play();
}

TEST(VideoDemos, Y4MVideoTest) {
  video vid;
  vid.loadY4M(vid_yuv_file, YUV420);
  vid.play();
}

TEST(Demos, ColorHist_Demo) {
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
