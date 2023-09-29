#include "image.hpp"

#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/core.hpp>

using namespace std;
using namespace cv;

image image::convert_BGR_YUV444() {
    int rows=image_mat_.rows;
    int cols=image_mat_.cols;

    Mat yPlane(rows,cols,CV_8UC1);
    Mat uPlane(rows,cols,CV_8UC1);
    Mat vPlane(rows,cols,CV_8UC1);

    for(int row=0;row<rows;row++){
        for(int col=0;col<cols;col++){
            Vec3b pixel=image_mat_.at<Vec3b>(row,col);
            u_char b= pixel.val[0];
            u_char g= pixel.val[1];
            u_char r= pixel.val[2];
            
            u_char y=0.299*r+0.587*g+0.114*b;
            u_char u=-0.14713*r - 0.28886*g + 0.436*b;
            u_char v=0.615*r - 0.51498*g - 0.10001*b;

            yPlane.at<u_char>(row,col)=y;
            uPlane.at<u_char>(row,col)=u;
            vPlane.at<u_char>(row,col)=v;
        }
    }

    Mat yuv(rows,cols,CV_8UC3);
    vector<Mat> channels;
    channels.push_back(yPlane);
    channels.push_back(uPlane);
    channels.push_back(vPlane);

    merge(channels,yuv);
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
            u_char b= pixel.val[0];
            u_char g= pixel.val[1];
            u_char r= pixel.val[2];

            u_char y=0.299*r+0.587*g+0.114*b;
            u_char u=-0.14713*r - 0.28886*g + 0.436*b;
            u_char v=0.615*r - 0.51498*g - 0.10001*b;

            yPlane.at<u_char>(row,col)=y;
            if(col%2==0) {
                uPlane.at<u_char>(row/2, col) = u;
                vPlane.at<u_char>(row/2, col) = v;
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
            u_char b= pixel.val[0];
            u_char g= pixel.val[1];
            u_char r= pixel.val[2];

            u_char y=0.299*r+0.587*g+0.114*b;
            u_char u=-0.14713*r - 0.28886*g + 0.436*b;
            u_char v=0.615*r - 0.51498*g - 0.10001*b;

            yPlane.at<u_char>(row,col)=y;
            if(col%2==0 and row%2==0) {
                uPlane.at<u_char>(row/2, col/2) = u;
                vPlane.at<u_char>(row/2, col/2) = v;
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

    Mat rPlane=Mat(rows,cols,CV_8UC1);
    Mat gPlane(rows,cols,CV_8UC1);
    Mat bPlane(rows,cols,CV_8UC1);

    for(int row=0;row<rows;row++){
        for(int col=0;col<cols;col++){
            Vec3b pixel=image_mat_.at<Vec3b>(row,col);
            u_char y= pixel.val[0];
            u_char u= pixel.val[1];
            u_char v= pixel.val[2];

            u_char r= y + 1.140*v;
            u_char g= y - 0.395*u - 0.581*v;
            u_char b= y + 2.032*u;

            rPlane.at<u_char>(row,col)=r;
            gPlane.at<u_char>(row,col)=g;
            bPlane.at<u_char>(row,col)=b;
        }
    }

    Mat bgr(rows,cols,CV_8UC3);
    vector<Mat> channels;
    channels.push_back(bPlane);
    channels.push_back(gPlane);
    channels.push_back(rPlane);

    merge(channels,bgr);
    image result;
    result._set_image_mat(bgr);
    result._set_format(BGR);
    return result;
}

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
    c_space = BGR;
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
