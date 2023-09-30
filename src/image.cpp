#include "image.hpp"

#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/core.hpp>

using namespace std;
using namespace cv;

image image::convert_BGR_YUV444() {
    int rows=image_mat_.rows;
    int cols=image_mat_.cols;

    cv::Mat yuv(rows, cols, CV_8UC3);

    for(int row=0;row<rows;row++){
        for(int col=0;col<cols;col++){
            Vec3b pixel=image_mat_.at<Vec3b>(row,col);
            double b= pixel.val[0];
            double g= pixel.val[1];
            double r= pixel.val[2];

            double y = 0.299 * r + 0.587 * g + 0.114 * b;
            double u = 0.492 * (b - y) + 128.0;
            double v = 0.877 * (r - y) + 128.0;

            // Set the YUV pixel values
            yuv.at<cv::Vec3b>(row,col) = cv::Vec3b(static_cast<uchar>(y), static_cast<uchar>(u), static_cast<uchar>(v));
        }
    }

    image result;
    result._set_image_mat(yuv);
    result._set_format(YUV444);
    return result;
}

image image::convert_BGR_YUV422() {
    int rows=image_mat_.rows;
    int cols=image_mat_.cols;

    Mat yPlane(rows,cols,CV_8UC1);
    Mat uPlane(rows,cols/2,CV_8UC1);
    Mat vPlane(rows,cols/2,CV_8UC1);

    for(int row=0;row<rows;row++){
        for(int col=0;col<cols;col++){
            Vec3b pixel=image_mat_.at<Vec3b>(row,col);
            double b= pixel.val[0];
            double g= pixel.val[1];
            double r= pixel.val[2];

            double y = 0.299 * r + 0.587 * g + 0.114 * b;
            double u = 0.492 * (b - y) + 128.0;
            double v = 0.877 * (r - y) + 128.0;

            yPlane.at<u_char>(row,col)=static_cast<uchar>(y);
            if(col%2==0) {
                uPlane.at<u_char>(row, col/2) = static_cast<uchar>(u);
                vPlane.at<u_char>(row, col/2) = static_cast<uchar>(v);
            }
        }
    }

    resize(uPlane,uPlane,Size(cols,rows));
    resize(vPlane,vPlane,Size(cols,rows));

    Mat yuv(rows,cols,CV_8UC3);
    vector<Mat> channels;
    channels.push_back(yPlane);
    channels.push_back(uPlane);
    channels.push_back(vPlane);

    merge(channels,yuv);
    image result;
    result._set_image_mat(yuv);
    result._set_format(YUV422);
    return result;
}

image image::convert_BGR_YUV420() {
    int rows=image_mat_.rows;
    int cols=image_mat_.cols;

    Mat yPlane(rows,cols,CV_8UC1);
    Mat uPlane(rows/2,cols/2,CV_8UC1);
    Mat vPlane(rows/2,cols/2,CV_8UC1);

    for(int row=0;row<rows;row++){
        for(int col=0;col<cols;col++){
            Vec3b pixel=image_mat_.at<Vec3b>(row,col);
            double b= pixel.val[0];
            double g= pixel.val[1];
            double r= pixel.val[2];

            double y = 0.299 * r + 0.587 * g + 0.114 * b;
            double u = 0.492 * (b - y) + 128.0;
            double v = 0.877 * (r - y) + 128.0;

            yPlane.at<u_char>(row,col)=static_cast<uchar>(y);
            if(row%2==0 && col%2==0) {
                uPlane.at<u_char>(row / 2, col / 2) = static_cast<uchar>(u);
                vPlane.at<u_char>(row / 2, col / 2) = static_cast<uchar>(v);
            }
        }
    }

    resize(uPlane,uPlane,Size(cols,rows));
    resize(vPlane,vPlane,Size(cols,rows));

    Mat yuv(rows,cols,CV_8UC3);
    vector<Mat> channels;
    channels.push_back(yPlane);
    channels.push_back(uPlane);
    channels.push_back(vPlane);

    merge(channels,yuv);
    image result;
    result._set_image_mat(yuv);
    result._set_format(YUV420);
    return result;
}

image image::convert_YUV_BGR() {
    int rows=image_mat_.rows;
    int cols=image_mat_.cols;

    Mat bgr(rows,cols,CV_8UC3);

    for(int row=0;row<rows;row++){
        for(int col=0;col<cols;col++){
            Vec3b pixel=image_mat_.at<Vec3b>(row,col);
            double y= pixel.val[0];
            double u= pixel.val[1]-128.0;
            double v= pixel.val[2]-128.0;

            double r = y + 1.13983 * v;
            double g = y - 0.39465 * u - 0.58060 * v;
            double b = y + 2.03211 * u;

            // Ensure values are within [0, 255]
            r = std::min(255.0, std::max(0.0, r));
            g = std::min(255.0, std::max(0.0, g));
            b = std::min(255.0, std::max(0.0, b));

            // Set the BGR pixel values
            bgr.at<cv::Vec3b>(row,col) = cv::Vec3b(static_cast<uchar>(b), static_cast<uchar>(g), static_cast<uchar>(r));
        }
    }

    image result;
    result._set_image_mat(bgr);
    result._set_format(BGR);
    return result;
}

image* image::load(Mat *arr) {
    image_mat_ = arr->clone();
    return this;
}

void image::load(const char* filename, ImreadModes mode)
{
    Mat image, conv;
	image = imread(filename, mode);
	if (!image.empty()) {
        image.convertTo(conv, CV_8UC3);
        image_mat_ = conv;
    }
    else {
        throw std::runtime_error("image has already been loaded");
    }
}

void image::save(const char* filename, const vector<int>& compression_params)
{
    if (loaded()) {
        imwrite(filename, image_mat_, compression_params);
    }
    else
    {
        throw std::runtime_error("image hasn't been loaded");
    }
}

void image::display_image(bool vid_ctx) {
    if (loaded()){
        imshow("image", image_mat_);
    }
    else
    {
        throw std::runtime_error("image hasn't been loaded");
    }
}

Vec3b image::get_pixel(int row, int col) const {
    if (loaded()) {
        if (row < 0 || row >= image_mat_.rows || col < 0 || col >= image_mat_.cols) {
            throw std::runtime_error("Pixel out of bounds");
        }
        Vec3b color_values = image_mat_.at<Vec3b>(row, col);
        return color_values;
    }
    throw std::runtime_error("image hasn't been loaded");
}

void image::set_pixel(int row, int col, Vec3b color_values) {
        if (row < 0 || row >= image_mat_.rows || col < 0 || col >= image_mat_.cols) {
            throw std::runtime_error("Pixel out of bounds");
        }
        image_mat_.at<Vec3b>(row, col) = color_values;
}



image image::clone() {
    if (loaded()) {
        image clone;
        clone.image_mat_ = image_mat_.clone();
        return clone;
    }
    else
        throw std::runtime_error("image hasn't been loaded");
}
