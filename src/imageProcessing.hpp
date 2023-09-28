//!  @file Declares functions that change the data of an object
#pragma once

#include "image.hpp"

using namespace std;
using namespace cv;

void watermark(image im, image *mark, Point2i coord1, Point2i coord2, double alpha);