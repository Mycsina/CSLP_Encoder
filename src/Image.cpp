#include "Image.h"

#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/core.hpp>

using namespace cv;

Image::Image()
= default;

Image::~Image()
= default;

bool Image::load(const char* filename)
{
	Mat I = imread(filename, IMREAD_COLOR);
	if (m_loaded_)
		image_mat_ = I;
	return I.empty();
}

void Image::display_image()
{
	if (m_loaded_) {
	imshow("Display Window", image_mat_);
	waitKey(0);
	}
	else
	{
		std::cout << "Image not loaded" << std::endl;
	}
}
