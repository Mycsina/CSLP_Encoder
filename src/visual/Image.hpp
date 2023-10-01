//! @file Image class declaration
/*!
 @brief Declares the Image class, representing a digital Image.
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

class Image {
private:
  Mat image_mat_;
  COLOR_SPACE c_space = BGR;
  CHROMA_SUBSAMPLING cs_ratio = NA;

public:
  Image() = default;
  ~Image() = default;
  void _set_image_mat(Mat mat) { image_mat_ = std::move(mat); }
  Mat *_get_image_mat() { return &image_mat_; }

  void _set_color(COLOR_SPACE col) { c_space = col; }

  //! Converts Image from BGR to YUV 4:4:4
  //! @return converted copy
  Image convert_BGR_YUV444();

  //! Converts Image from BGR to YUV 4:2:2
  //! @return converted copy
  Image convert_BGR_YUV422();

  //! Converts Image from BGR to YUV 4:2:0
  //! @return converted copy
  Image convert_BGR_YUV420();

  //! Converts Image from YUV to BGR
  //! @return converted copy
  Image convert_YUV_BGR();

  COLOR_SPACE _get_color() { return c_space; }

  void _set_chroma(CHROMA_SUBSAMPLING cs) {
    if (c_space == YUV)
      cs_ratio = cs;
    else
      throw std::runtime_error(
          "Chroma subsampling only makes sense in YUV color space");
  }

  CHROMA_SUBSAMPLING _get_chroma() { return cs_ratio; }

  //! Used to iterate over all pixels in the Image
  //! @return Iterator to the first pixel in the Image
  MatIterator_<Vec3b> begin() { return image_mat_.begin<Vec3b>(); }

  //! Used to iterate over all pixels in the Image
  //! @return Iterator to the last pixel in the Image
  MatIterator_<Vec3b> end() { return image_mat_.end<Vec3b>(); }

  //! Loads an Image from a cv::Mat
  //! @return Image file
  Image *load(Mat *arr2d);

  explicit Image(Mat *arr2d) {
    load(arr2d);
    c_space = BGR;
  }

  //! Return size of Image
  //! @return Array of integers containing the size of the Image in the format
  //! <rows, cols>
  array<int, 2> size() const { return {image_mat_.rows, image_mat_.cols}; }

  //! Loads an Image from a file
  //! \details The Image is stored in a 8-bit unsigned integer matrix with 3
  //! channels (BGR)
  //! @param  filename Absolute path to Image file
  //! @param  mode Mode to load Image in (see cv:ImreadModes for available
  //! modes)
  void load(const basic_string<char> &filename,
            ImreadModes mode = IMREAD_COLOR);

  //! Saves an Image to a file
  //! @param  filename Absolute path to Image file
  //! @param  compression_params Vector of integers specifying the compression
  //! parameters with format <paramID, paramValue> (see cv:ImwriteFlags for
  //! available parameters)
  void save(const char *filename, const vector<int> &compression_params = {});

  //! Get size of Image
  //! @return Array of integers containing the size of the Image in the format
  //! <rows, cols>
  array<int, 2> get_image_size() const {
    return {image_mat_.rows, image_mat_.cols};
  }

  //! Get data type of Image
  //! @return Integer indicating the data type of the Image
  int get_image_type() const { return image_mat_.type(); }

  //! Displays the Image in a window
  //! @params vid_ctx Indicates whether it is to be used in displaying a video
  void display_image(bool vid_ctx = false);

  //! Returns whether the Image has been loaded
  //! @return Boolean indicating whether the Image has been loaded
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

  //! Get a deep copy of the Image
  //! @return Image object containing a deep copy of the Image
  Image clone();

  //! Create color histograms of a BGR Image
  //! @param  hist Histogram to be displayed
  //! @param  fill_hist Indicates whether the histogram should be filled
  //! @param  width Width in pixels of the histogram
  //! @param  height Height in pixels of the histogram
  //! @return Vector with resulting Histograms
  vector<Mat> color_histograms(int bins = 256, bool fill_hist = false,
                               int width = 512, int height = 400);

  //! Apply the gaussian blur filter to this image
  //! @param blur the blur coefficient matrix (values are int)
  //! @return Copy of image with the blur applied
  Image gaussian_blur(Mat blur);

  //! Copies a portion of the image matrix (values out of range are excluded)
  //! @param radiusR vertical radius
  //! @param radiusC horizontal radius
  //! @param r,c the coordinates of the central pixel
  //! @return The selected submatrix
  Mat get_neighbors(int radiusR, int radiusC,int r,int c);
};
