#include "image.hpp"

#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/core.hpp>

using namespace std;
using namespace cv;


image* image::load(Mat *arr) {
    image_mat_ = arr->clone();
    return this;
}

void image::load(const basic_string<char> &filename, ImreadModes mode)
{
    Mat image, conv;
    // By default, imread loads images in BGR format
    //TODO add support for other formats | currently everything gets converted to BGR format and all operations expect a BGR image
	image = imread(filename, mode);
    color = BGR;
	if (!image.empty()) {
        image.convertTo(conv, CV_8UC3);
        image_mat_ = conv;
    }
    else {
        throw std::runtime_error("Image has already been loaded");
    }
}

void image::save(const char* filename, const vector<int>& compression_params)
{
    if (loaded()) {
        imwrite(filename, image_mat_, compression_params);
    }
    else
    {
        throw std::runtime_error("Image hasn't been loaded");
    }
}

void image::display_image(bool vid_ctx) {
    if (loaded()) {
        imshow("Image", image_mat_);
        if (!vid_ctx)
            waitKey(0);
        else
            waitKey(25);
    }
    else
    {
        throw std::runtime_error("Image hasn't been loaded");
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
    throw std::runtime_error("Image hasn't been loaded");
}

void image::set_pixel(int row, int col, const Vec3b& color_values) {
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
        throw std::runtime_error("Image hasn't been loaded");
}
