#include "../src/visual/Image.hpp"
#include "../src/visual/ImageProcessing.hpp"
#include <gtest/gtest.h>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

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

TEST_F(ImageDemo, SubSampling_Demo) {
    im.show();

    im2 = convert_BGR_YUV444(im);
    im2 = convert_YUV_BGR(im2);
    im2.show();

    im2 = convert_BGR_YUV422(im);
    im2 = convert_YUV_BGR(im2);
    im2.show();

    im2 = convert_BGR_YUV420(im);
    im2 = convert_YUV_BGR(im2);
    im2.show();
}

TEST_F(ImageDemo, ColorHist_Demo) {
    im.show();
    // im = convert_BGR_GRAY(im);
    vector<Mat> hists = im.color_histograms(256, true, 1024, 800);
    // NOTE I found that if you try to allocate a new array with same shape and
    // type, it will return the previous array, so it's needed to assure that it
    // is filled with zeroes
    Mat carry = Mat::zeros(hists[0].rows, hists[0].cols, CV_8UC3);
    for (auto &hist: hists) {
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

TEST_F(ImageDemo, EqualizedColorHist_Demo) {
    im.show();
    equalize_hist(im);
    im.show();
    // im = convert_BGR_GRAY(im);
    vector<Mat> hists = im.color_histograms(256, true, 1024, 800);
    // NOTE I found that if you try to allocate a new array with same shape and
    // type, it will return the previous array, so it's needed to assure that it
    // is filled with zeroes
    Mat carry = Mat::zeros(hists[0].rows, hists[0].cols, CV_8UC3);
    for (auto &hist: hists) {
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
    auto mark_file = "../../tests/resource/lena.ppm";
    Image mark(mark_file);
    watermark(im, mark, {0, 0},
              {mark.size()[1], mark.size()[0]}, 0.5);
    im.show();
}

TEST_F(ImageDemo, SubsamplingDemo) {
    im = convert_BGR_YUV444(im);
    im2 = convert_BGR_YUV444(im2);
    im.show();
    subsample(im, YUV422);
    im.show();
    subsample(im2, YUV420);
    im2.show();
}

TEST_F(ImageDemo, ThresholdDemo) {
    Image img("../../tests/resource/otsu.jpg");
    img.show();
    binarize(img);
    img.show();
}

TEST_F(ImageDemo, GaussianDemoFlat1) {
    // 1  1  1
    // 1  1  1
    // 1  1  1
    im.show();
    Mat blur(3, 3, CV_8UC3, Scalar(1, 1, 1));
    im.gaussian_blur(blur).show();
}

TEST_F(ImageDemo, GaussianDemoFlat2) {
    //[11x11]
    im.show();
    Mat blur(11, 11, CV_8UC3, Scalar(1, 1, 1));
    im.gaussian_blur(blur).show();
}

TEST_F(ImageDemo, GaussianDemoDistanceBased1) {
    // 1  2  1
    // 2  3  2
    // 1  2  1
    Mat blur(3, 3, CV_8UC3, Scalar(1, 1, 1));
    blur.at<Vec3b>(1, 1) = Vec3b(3, 3, 3);

    blur.at<Vec3b>(0, 1) = Vec3b(2, 2, 2);
    blur.at<Vec3b>(1, 0) = Vec3b(2, 2, 2);
    blur.at<Vec3b>(2, 1) = Vec3b(2, 2, 2);
    blur.at<Vec3b>(1, 2) = Vec3b(2, 2, 2);
    im.show();
    im.gaussian_blur(blur).show();
}
