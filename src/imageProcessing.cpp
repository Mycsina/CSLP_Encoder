#include "imageProcessing.hpp"

using namespace std;
using namespace cv;

void watermark(image im, image *mark, Point2i coord1, Point2i coord2, double alpha) {
    Mat imageMat = *im._get_image_mat();
    Mat markMat = *mark->_get_image_mat();
    int height = coord2.y - coord1.y;
    int width = coord2.x - coord1.x;
    // Resize the watermark to fit the desired area
    InterpolationFlags flag;
    if (height < markMat.rows && width < markMat.cols) {
        flag = INTER_AREA;
    } else {
        flag = INTER_LINEAR;
    }
    resize(markMat, markMat, Size(width, height), 0 ,0, flag);
    // Blend the watermark with the image
    Mat roi = imageMat(Rect(coord1.x, coord1.y, width, height));
    addWeighted(roi, alpha, markMat, 1.0 - alpha, 0.0, roi);
}

//! Convert an image from BGR to YUV color space (as explained in wikipedia)
//! @param matrix Pointer to the matrix to be converted
void BGR2YUV(Mat *matrix) {
    if (matrix->channels() != 3) {
        throw std::runtime_error("Original matrix must have 3 channels");
    }
    if (matrix->depth() != CV_8U) {
        throw std::runtime_error("Original matrix must have 8-bit unsigned integers");
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

//! Convert an image from YUV to BGR color space (as explained in wikipedia)
//! @param matrix Pointer to the matrix to be converted
void YUV2BGR(Mat *matrix) {
    if (matrix->channels() != 3) {
        throw std::runtime_error("Original matrix must have 3 channels");
    }
    if (matrix->depth() != CV_8U) {
        throw std::runtime_error("Original matrix must have 8-bit unsigned integers");
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

// TODO are we supposed to implement this too?
void biline_interp(Mat *matrix, int width, int height) {

}