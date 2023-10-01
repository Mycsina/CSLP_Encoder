#include "Image.hpp"

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Image Image::convert_BGR_YUV444()
{
  int rows = image_mat_.rows;
  int cols = image_mat_.cols;

  cv::Mat yuv(rows, cols, CV_8UC3);


    image_mat_.forEach<Vec3b>(
            [&yuv](Vec3b &pixel, const int *position){
                int row=position[0];
                int col=position[1];

                double b = pixel.val[0];
                double g = pixel.val[1];
                double r = pixel.val[2];

                double y = 0.299 * r + 0.587 * g + 0.114 * b;
                double u = 0.492 * (b - y) + 128.0;
                double v = 0.877 * (r - y) + 128.0;

                yuv.at<cv::Vec3b>(row, col) = cv::Vec3b(
                        static_cast<uchar>(y), static_cast<uchar>(u), static_cast<uchar>(v));
            });

  Image result;
  result._set_image_mat(yuv);
  result._set_color(YUV);
  result._set_chroma(YUV444);
  return result;
}

Image Image::convert_BGR_YUV422()
{
  int rows = image_mat_.rows;
  int cols = image_mat_.cols;

  Mat yPlane(rows, cols, CV_8UC1);
  Mat uPlane(rows, cols / 2, CV_8UC1);
  Mat vPlane(rows, cols / 2, CV_8UC1);

    image_mat_.forEach<Vec3b>(
            [&yPlane, &uPlane, &vPlane](Vec3b &pixel, const int *position){
                int row=position[0];
                int col=position[1];

                double b = pixel.val[0];
                double g = pixel.val[1];
                double r = pixel.val[2];

                double y = 0.299 * r + 0.587 * g + 0.114 * b;
                double u = 0.492 * (b - y) + 128.0;
                double v = 0.877 * (r - y) + 128.0;

                yPlane.at<u_char>(row, col) = static_cast<uchar>(y);
                if (col % 2 == 0)
                {
                    uPlane.at<u_char>(row, col / 2) = static_cast<uchar>(u);
                    vPlane.at<u_char>(row , col / 2) = static_cast<uchar>(v);
                }
            });

  resize(uPlane, uPlane, Size(cols, rows));
  resize(vPlane, vPlane, Size(cols, rows));

  Mat yuv(rows, cols, CV_8UC3);
  vector<Mat> channels;
  channels.push_back(yPlane);
  channels.push_back(uPlane);
  channels.push_back(vPlane);

  merge(channels, yuv);
  Image result;
  result._set_image_mat(yuv);
  result._set_color(YUV);
  result._set_chroma(YUV422);
  return result;
}

Image Image::convert_BGR_YUV420()
{
  int rows = image_mat_.rows;
  int cols = image_mat_.cols;

  Mat yPlane(rows, cols, CV_8UC1);
  Mat uPlane(rows / 2, cols / 2, CV_8UC1);
  Mat vPlane(rows / 2, cols / 2, CV_8UC1);

  image_mat_.forEach<Vec3b>(
          [&yPlane, &uPlane, &vPlane](Vec3b &pixel, const int *position){
              int row=position[0];
              int col=position[1];

              double b = pixel.val[0];
              double g = pixel.val[1];
              double r = pixel.val[2];

              double y = 0.299 * r + 0.587 * g + 0.114 * b;
              double u = 0.492 * (b - y) + 128.0;
              double v = 0.877 * (r - y) + 128.0;

              yPlane.at<u_char>(row, col) = static_cast<uchar>(y);
              if (row % 2 == 0 && col % 2 == 0)
              {
                  uPlane.at<u_char>(row / 2, col / 2) = static_cast<uchar>(u);
                  vPlane.at<u_char>(row / 2, col / 2) = static_cast<uchar>(v);
              }
          });

  resize(uPlane, uPlane, Size(cols, rows));
  resize(vPlane, vPlane, Size(cols, rows));

  Mat yuv(rows, cols, CV_8UC3);
  vector<Mat> channels;
  channels.push_back(yPlane);
  channels.push_back(uPlane);
  channels.push_back(vPlane);

  merge(channels, yuv);
  Image result;
  result._set_image_mat(yuv);
  result._set_color(YUV);
  result._set_chroma(YUV420);
  return result;
}

Image Image::convert_YUV_BGR()
{
  int rows = image_mat_.rows;
  int cols = image_mat_.cols;

  Mat bgr(rows, cols, CV_8UC3);

  image_mat_.forEach<Vec3b>(
          [&bgr](Vec3b& pixel, const int* position) -> void {
              int row=position[0];
              int col=position[1];

              double y = pixel.val[0];
              double u = pixel.val[1] - 128.0;
              double v = pixel.val[2] - 128.0;

              double r = y + 1.13983 * v;
              double g = y - 0.39465 * u - 0.58060 * v;
              double b = y + 2.03211 * u;

              // Ensure values are within [0, 255]
              r = std::min(255.0, std::max(0.0, r));
              g = std::min(255.0, std::max(0.0, g));
              b = std::min(255.0, std::max(0.0, b));

              // Set the BGR pixel values
              bgr.at<cv::Vec3b>(row, col) = cv::Vec3b(
                      static_cast<uchar>(b), static_cast<uchar>(g), static_cast<uchar>(r));
          });
  Image result;
  result._set_image_mat(bgr);
  result._set_color(BGR);
  return result;
}

Image *Image::load(const Mat &arr) {
  image_mat_ = arr.clone();
  return this;
}

void Image::load(const basic_string<char> &filename, ImreadModes mode)
{
  Mat image, conv;
  // By default, imread loads images in BGR format
  // TODO add support for other formats | currently everything gets converted to
  // BGR format and all operations expect a BGR Image
  image = imread(filename, mode);
  c_space = BGR;
  if (!image.empty())
  {
    image.convertTo(conv, CV_8UC3);
    image_mat_ = conv;
  }
  else
  {
    throw std::runtime_error("Image has already been loaded");
  }
}

void Image::save(const basic_string<char> &filename,
                 const vector<int> &compression_params) {
  if (loaded())
  {
    imwrite(filename, image_mat_, compression_params);
  }
  else
  {
    throw std::runtime_error("Image hasn't been loaded");
  }
}

void Image::display_image(bool vid_ctx)
{
  if (loaded())
  {
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

Vec3b Image::get_pixel(int row, int col) const
{
  if (loaded())
  {
    if (row < 0 || row >= image_mat_.rows || col < 0 ||
        col >= image_mat_.cols)
    {
      throw std::runtime_error("Pixel out of bounds");
    }
    Vec3b color_values = image_mat_.at<Vec3b>(row, col);
    return color_values;
  }
  throw std::runtime_error("Image hasn't been loaded");
}

void Image::set_pixel(int row, int col, const Vec3b &color_values)
{
  if (row < 0 || row >= image_mat_.rows || col < 0 || col >= image_mat_.cols)
  {
    throw std::runtime_error("Pixel out of bounds");
  }
  image_mat_.at<Vec3b>(row, col) = color_values;
}

Image Image::clone()
{
  if (loaded())
  {
    Image clone;
    clone.image_mat_ = image_mat_.clone();
    return clone;
  }
  else
    throw std::runtime_error("Image hasn't been loaded");
}

vector<Mat> Image::color_histograms(int bins, bool fill_hist, int width,
                                    int height)
{
  if (loaded())
  {
    vector<Mat> histograms;
    vector<Mat> channels;
    // Array of colors for each channel
    Scalar colors[] = {Scalar(255, 0, 0), Scalar(0, 255, 0), Scalar(0, 0, 255)};
    split(image_mat_, channels);
    // If image is grayscale, set color to white
    if (channels.size() == 1) {
      colors[0] = Scalar(255, 255, 255);
    }
    for (int i = 0; i < channels.size(); i++)
    {
      Histogram hist = Histogram(bins);
      hist.color = colors[i];
      Mat backMat = histogram<uchar>(channels[i], bins);
      int bin_w = cvRound((double)width / 256);
      Vec3b backColor = (0, 0, 0);

      Mat histImage(height, width, CV_8UC3, backColor);

      normalize(backMat, backMat, 0, histImage.rows, NORM_MINMAX, -1, Mat());

      for (int j = 1; j < 256; j++)
      {
        line(histImage,
             Point(bin_w * (j - 1), height - cvRound(backMat.at<float>(j - 1))),
             Point(bin_w * (j), height - cvRound(backMat.at<float>(j))),
             hist.color, 2, 8, 0);
      }

      if (fill_hist)
      {
        for (int j = 0; j < histImage.cols; j++)
        {
          Mat col = histImage.col(j);
          for (int k = col.rows - 1; k >= 0; k--)
          {
            // Fill column with color until a non-<backColor> pixel is found
            if (col.at<Vec3b>(k) != backColor)
            {
              break;
            }
            Vec3b color = {(uchar)hist.color[0], (uchar)hist.color[1],
                           (uchar)hist.color[2]};
            col.at<Vec3b>(k) = color;
            // If we reach end of column without finding a non-<backColor> pixel
            // zero the column
            if (k == 0)
            {
              col = Mat::zeros(col.rows, 1, CV_8UC3);
            }
          }
        }
      }

      histograms.push_back(histImage);
    }
    return histograms;
  }
  else
    throw std::runtime_error("Image hasn't been loaded");
}

Image Image::gaussian_blur(cv::Mat blur) {
    Image i=this->clone();
    Mat *m=i._get_image_mat();
    int totalBlur=int(sum(blur)[0]);
    int radiusR,radiusC;

    //Check if the blur matrix has proper dimensions
    if(blur.rows%2==0 || blur.cols%2==0) {
        throw std::invalid_argument("blur matrix must have odd rows and columns");
    }
    radiusR=blur.rows/2;
    radiusC=blur.cols/2;

    //iterate every pixel to apply the filter
    image_mat_.forEach<Vec3b>(
            [&blur,radiusC,radiusR,m, this, totalBlur](Vec3b& pixel, const int* position){
                int row=position[0];
                int col=position[1];

                Mat blur_temp=cut(blur,row,col); //cut the blur filter as needed
                Mat temp=get_neighbors(radiusR,radiusC,row,col); //get the neighbors

                temp=temp.mul(blur_temp);
                cv::Scalar sum=cv::sum(temp);

                //normalize
                for(int a=0;a<sum.rows;a++){
                    pixel[a]=(int)(sum[a]/totalBlur);
                }
                m->at<Vec3b>(row,col)=pixel;
            });
    return i;
}

Mat Image::get_neighbors(int radiusR, int radiusC,int r, int c) {
    if(r<0 || r>=image_mat_.rows || c<0 || c>=image_mat_.cols){
        throw std::out_of_range("Pixel out of bounds");
    }

    //get the min and max values, making sure to stay in bounds
    int rMin=max(0,r-radiusR);
    int rMax=min(image_mat_.rows-1,r+radiusR);
    int cMin=max(0,c-radiusC);
    int cMax=min(image_mat_.cols-1,c+radiusC);

    return image_mat_(Range(rMin,rMax+1),Range(cMin,cMax+1));
}

Mat Image::cut(Mat m, int row, int col) {
    int centerRow=m.rows/2;
    int centerCol=m.cols/2;
    //check distance in image_mat from this pixel to the edge
    int distUp = row - 0;
    int distDown = (image_mat_.rows - 1) - row;
    int distLeft = col - 0;
    int distRight = (image_mat_.cols - 1) - col;

    //check for need to cut
    int rMin=max((centerRow-distUp),0);
    int rMax=min((centerRow+distDown),m.rows-1);
    int cMin=max((centerCol-distLeft),0);
    int cMax=min((centerCol+distRight),m.cols-1);

    return m(Range(rMin,rMax+1),Range(cMin,cMax+1));
}