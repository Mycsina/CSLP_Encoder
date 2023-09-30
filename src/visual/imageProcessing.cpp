#include "imageProcessing.hpp"
#include <iostream>

using namespace std;
using namespace cv;

// TODO how manually are we supposed to implement this?

void watermark(Image &im, Image mark, Point2i coord1, Point2i coord2,
               double alpha) {
  Mat imageMat = *im._get_image_mat();
  Mat markMat = *mark._get_image_mat();
  int height = coord2.y - coord1.y;
  int width = coord2.x - coord1.x;
  // Resize the watermark to fit the desired area
  InterpolationFlags flag;
  if (height < markMat.rows && width < markMat.cols) {
    flag = INTER_AREA;
  } else {
    flag = INTER_LINEAR;
  }
  resize(markMat, markMat, Size(width, height), 0, 0, flag);
  // Blend the watermark with the Image
  Mat roi = imageMat(Rect(coord1.x, coord1.y, width, height));
  addWeighted(roi, alpha, markMat, 1.0 - alpha, 0.0, roi);
}

void BGR2YUV(Image &im) {
  Mat *matrix = im._get_image_mat();
  if (matrix->channels() != 3) {
    throw std::runtime_error("Original matrix must have 3 channels");
  }
  if (matrix->depth() != CV_8U) {
    throw std::runtime_error(
        "Original matrix must have 8-bit unsigned integers");
  }
  for (int i = 0; i < matrix->rows; i++) {
    for (int j = 0; j < matrix->cols; j++) {
      Vec3b color = matrix->at<Vec3b>(i, j);
      int R = color[2], G = color[1], B = color[0];
      uchar Y = 0.299 * R + 0.587 * G + 0.114 * B;
      uchar U = (R - Y) * 0.492;
      uchar V = (B - Y) * 0.877;
      matrix->at<Vec3b>(i, j) = {Y, U, V};
    }
  }
}

void YUV2BGR(Image &im) {
  Mat *matrix = im._get_image_mat();
  if (matrix->channels() != 3) {
    throw std::runtime_error("Original matrix must have 3 channels");
  }
  if (matrix->depth() != CV_8U) {
    throw std::runtime_error(
        "Original matrix must have 8-bit unsigned integers");
  }
  for (int i = 0; i < matrix->rows; i++) {
    for (int j = 0; j < matrix->cols; j++) {
      Vec3b color = matrix->at<Vec3b>(i, j);
      int Y = color[0], U = color[1], V = color[2];
      uchar R = Y + 1.140 * V;
      uchar G = Y - 0.395 * U - 0.581 * V;
      uchar B = Y + 2.032 * U;
      matrix->at<Vec3b>(i, j) = {B, G, R};
    }
  }
}

void BGR2GRAY(Image &im) {
  Mat *matrix = im._get_image_mat();
  Mat gray(matrix->rows, matrix->cols, CV_8UC1);
  if (matrix->channels() != 3) {
    throw std::runtime_error("Original matrix must have 3 channels");
  }
  if (matrix->depth() != CV_8U) {
    throw std::runtime_error(
        "Original matrix must have 8-bit unsigned integers");
  }
  for (int i = 0; i < matrix->rows; i++) {
    for (int j = 0; j < matrix->cols; j++) {
      Vec3b color = matrix->at<Vec3b>(i, j);
      int R = color[2], G = color[1], B = color[0];
      uchar Y = 0.299 * R + 0.587 * G + 0.114 * B;
      gray.at<uchar>(i, j) = Y;
    }
  }
  im._set_image_mat(gray);
  im._set_color(GRAY);
}

//! Subsample the non-luma channels of an Image
//! @param im Image to be subsampled
//! @param ratio Subsampling ratio
void subsample(Image &im, CHROMA_SUBSAMPLING ratio) {
  Mat *matrix = im._get_image_mat();
  Mat channels[3];
  split(*matrix, channels);
  Size target_size = Size(channels[0].size[1], channels[0].size[0]);
  float scaling[2];
  if (matrix->cols % 2 != 0) {
    throw std::runtime_error("Matrix must have an even number of columns");
  }
  switch (ratio) {
  case YUV444:
    cout << "No subsampling has been performed" << endl;
    break;
  case YUV422:
    // 1/2 horizontal sampling, full vertical sampling
    scaling[0] = 0.5;
    scaling[1] = 1;
    break;
  case YUV420:
    // 1/2 horizontal sampling, 1/2 vertical sampling
    scaling[0] = 0.5;
    scaling[1] = 0.5;
    break;
  default:
    throw std::runtime_error("Invalid subsampling ratio");
  }
  resize(channels[1], channels[1], target_size, scaling[0], scaling[1],
         INTER_LINEAR);
  resize(channels[2], channels[2], target_size, scaling[0], scaling[1],
         INTER_LINEAR);
  merge(channels, 3, *matrix);
}

// TODO are we supposed to implement this too?
void biline_interp(Mat &matrix, int width, int height) {}

void equalize_hist(Image &im) {
  Mat *matrix = im._get_image_mat();
  vector<Mat> channels;
  split(*matrix, channels);
  if (matrix->depth() != CV_8U) {
    throw std::runtime_error(
        "Original matrix must have 8-bit unsigned integers");
  }
  for (auto &channel : channels) {
    equalizeHist(channel, channel);
  }
  merge(channels, *matrix);
}

Mat ecdf(const Mat &histogram, int total) {
  Mat res = Mat::zeros(histogram.rows, histogram.cols, CV_32F);
  float cumsum = 0;
  for (int j = 0; j < histogram.cols; j++) {
    cumsum += histogram.at<float>(j);
    float val = cumsum / (float)total;
    res.at<float>(j) = val;
  }
  return res;
}

void binarize(Image &im) {
  Mat matrix = *im._get_image_mat();
  int pix_num = matrix.rows * matrix.cols;
  int bins = 256;
  if (matrix.depth() != CV_8U) {
    throw std::runtime_error(
        "Original matrix must have 8-bit unsigned integers");
  }
  if (matrix.channels() == 3) {
    cout << "Converting to grayscale" << endl;
    BGR2GRAY(im);
    matrix = *im._get_image_mat();
  }
  Mat hist = histogram<uchar>(matrix, bins);
  // Get empirical cumulative distribution function
  Mat distrib = ecdf(hist, pix_num);
  // Get best threshold
  int best_threshold = 0;
  double max_variance = 0;
  for (int thresh = 1; thresh < bins - 1; thresh++) {
    // Calculate percentage of pixels above and below threshold (foreground and
    // background)
    // w0
    double fg_prob = distrib.at<float>(thresh);
    // w1
    double bg_prob = 1 - fg_prob;
    // Skip if one of the percentages is 0
    if (fg_prob == 0 || bg_prob == 0) {
      continue;
    }
    // μ0 and μ1
    float fg_mean = 0, bg_mean = 0;
    for (int i = 0; i < thresh; i++) {
      fg_mean += i * hist.at<float>(i);
    }
    for (int i = thresh; i < bins; i++) {
      bg_mean += i * hist.at<float>(i);
    }
    fg_mean /= fg_prob;
    bg_mean /= bg_prob;
    double inter_variance =
        fg_prob * bg_prob * ((fg_mean - bg_mean) * (fg_mean - bg_mean));
    if (inter_variance > max_variance) {
      max_variance = inter_variance;
      best_threshold = thresh;
    }
  }
  // Binarize image
  matrix.forEach<uchar>(
      [&best_threshold](uchar &pixel, const int *position) -> void {
        if (pixel > best_threshold) {
          pixel = 255;
        } else {
          pixel = 0;
        }
      });
}
