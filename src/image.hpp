//! @file image class declaration
/*!
 @brief Declares the image class, representing a digital image.
*/
#pragma once
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <utility>

#include "Histogram.cpp"

using namespace std;
using namespace cv;

/* TODO
    Converting between color spaces.
    */

enum COLOR_SPACE { BGR, YUV, GRAY };

enum CHROMA_SUBSAMPLING { NA, YUV444, YUV422, YUV420 };

class image {
private:
  Mat image_mat_;
  COLOR_SPACE c_space = BGR;
  CHROMA_SUBSAMPLING cs_ratio = NA;

public:
  image() = default;
  ~image() = default;
  void _set_image_mat(Mat mat) { image_mat_ = std::move(mat); }
  Mat *_get_image_mat() { return &image_mat_; }

  void _set_color(COLOR_SPACE col) { c_space = col; }

  //! Converts image from BGR to YUV 4:4:4
  //! @return converted copy
  image convert_BGR_YUV444();

  //! Converts image from BGR to YUV 4:2:2
  //! @return converted copy
  image convert_BGR_YUV422();

  //! Converts image from BGR to YUV 4:2:0
  //! @return converted copy
  image convert_BGR_YUV420();

  //! Converts image from YUV to BGR
  //! @return converted copy
  image convert_YUV_BGR();

  COLOR_SPACE _get_color() { return c_space; }

  void _set_chroma(CHROMA_SUBSAMPLING cs) {
    if (c_space == YUV)
      cs_ratio = cs;
    else
      throw std::runtime_error(
          "Chroma subsampling only makes sense in YUV color space");
  }

  CHROMA_SUBSAMPLING _get_chroma() { return cs_ratio; }

  //! Used to iterate over all pixels in the image
  //! @return Iterator to the first pixel in the image
  MatIterator_<Vec3b> begin() { return image_mat_.begin<Vec3b>(); }

  //! Used to iterate over all pixels in the image
  //! @return Iterator to the last pixel in the image
  MatIterator_<Vec3b> end() { return image_mat_.end<Vec3b>(); }

  //! Loads an image from a cv::Mat
  //! @return Image file
  image *load(Mat *arr2d);

  explicit image(Mat *arr2d) {
    load(arr2d);
    c_space = BGR;
  }

  //! Return size of image
  //! @return Array of integers containing the size of the image in the format
  //! <rows, cols>
  array<int, 2> size() const { return {image_mat_.rows, image_mat_.cols}; }

  //! Loads an image from a file
  //! \details The image is stored in a 8-bit unsigned integer matrix with 3
  //! channels (BGR)
  //! @param  filename Absolute path to image file
  //! @param  mode Mode to load image in (see cv:ImreadModes for available
  //! modes)
  void load(const basic_string<char> &filename,
            ImreadModes mode = IMREAD_COLOR);

  //! Saves an image to a file
  //! @param  filename Absolute path to image file
  //! @param  compression_params Vector of integers specifying the compression
  //! parameters with format <paramID, paramValue> (see cv:ImwriteFlags for
  //! available parameters)
  void save(const char *filename, const vector<int> &compression_params = {});

  //! Get size of image
  //! @return Array of integers containing the size of the image in the format
  //! <rows, cols>
  array<int, 2> get_image_size() const {
    return {image_mat_.rows, image_mat_.cols};
  }

  //! Get data type of image
  //! @return Integer indicating the data type of the image
  int get_image_type() const { return image_mat_.type(); }

  //! Displays the image in a window
  //! @params vid_ctx Indicates whether it is to be used in displaying a video
  void display_image(bool vid_ctx = false);

  //! Returns whether the image has been loaded
  //! @return Boolean indicating whether the image has been loaded
  bool loaded() const { return !image_mat_.empty(); }

  //! Get color values from a pixel (0-255 range)
  //! @param  row Row of pixel / y-coordinate
  //! @param  col Column of pixel / x-coordinate
  //! @return Array of integers containing the color values of the pixel in the
  Vec3b get_pixel(int row, int col) const;

  //! Set color values of a pixel (0-255 range)
  //! @param  row Row of pixel / y-coordinate
  //! @param  col Column of pixel / x-coordinate
  //! @param  color_values Array of integers containing the color values of the
  //! pixel
  void set_pixel(int row, int col, const Vec3b &color_values);

  //! Get a deep copy of the image
  //! @return image object containing a deep copy of the image
  image clone();

  //! Create color histograms of a BGR image
  //! @param  hist Histogram to be displayed
  //! @param  fill_hist Indicates whether the histogram should be filled
  //! @param  width Width in pixels of the histogram
  //! @param  height Height in pixels of the histogram
  //! @return Vector with resulting Histograms
  vector<Mat> color_histograms(int bins = 256, bool fill_hist = false,
                               int width = 512, int height = 400);
};
