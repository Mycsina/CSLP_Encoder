#include "Image.hpp"

#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/core.hpp>

using namespace std;
using namespace cv;

void Image::load(const char* filename, ImreadModes mode)
{
    Mat image, conv;
	image = imread(filename, mode);
	if (!loaded()) {
        image.convertTo(conv, CV_8UC3);
        image_mat_ = conv;
    }
    else
        throw std::runtime_error("Image has already been loaded");
}

void Image::save(const char* filename, const vector<int>& compression_params)
{
    if (loaded()) {
        imwrite(filename, image_mat_, compression_params);
    }
    else
    {
        throw std::runtime_error("Image hasn't been loaded");
    }
}

void Image::display_image()
{
	if (loaded()) {
	imshow("Image", image_mat_);
	waitKey(0);
	}
	else
	{
        throw std::runtime_error("Image hasn't been loaded");
	}
}

Vec3b Image::get_pixel(int row, int col) const {
    if (loaded()) {
        if (row < 0 || row >= image_mat_.rows || col < 0 || col >= image_mat_.cols) {
            throw std::runtime_error("Pixel out of bounds");
        }
        Vec3b color_values = image_mat_.at<Vec3b>(row, col);
        return color_values;
    }
    throw std::runtime_error("Image hasn't been loaded");
}

void Image::set_pixel(int row, int col, Vec3b color_values) {
    if (loaded()) {
        if (row < 0 || row >= image_mat_.rows || col < 0 || col >= image_mat_.cols) {
            throw std::runtime_error("Pixel out of bounds");
        }
        image_mat_.at<Vec3b>(row, col) = color_values;
    }
    else
        throw std::runtime_error("Image hasn't been loaded");
}

Image Image::clone() {
    if (loaded()) {
        Image clone;
        clone.image_mat_ = image_mat_.clone();
        return clone;
    }
    else
        throw std::runtime_error("Image hasn't been loaded");
}