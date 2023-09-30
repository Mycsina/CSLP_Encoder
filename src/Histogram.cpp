#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class Histogram {
public:
  Scalar color = Scalar(255, 255, 255);
  int bins_;
  Mat mat_;
  Histogram(int bins) {
    bins_ = bins;
    mat_ = Mat::zeros(bins_, 1, CV_32SC1);
  }
};