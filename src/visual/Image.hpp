﻿/**
 * @file Image.hpp
 * @brief Image class
 * @ingroup Visual
 */

#pragma once

#include "../io/Golomb.hpp"
#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>

enum COLOR_SPACE : std::uint8_t {
    BGR,//!< Blue-Green-Red
    YUV,//!< Luminance-Chrominance
    GRAY//!< Grayscale
};

enum CHROMA_SUBSAMPLING : std::uint8_t {
    NA,    //!< Not applicable
    YUV444,//!< 4:4:4
    YUV422,//!< 4:2:2
    YUV420 //!< 4:2:0
};

//! @brief Histogram struct declaration
struct Histogram {
    cv::Scalar color = cv::Scalar(255, 255, 255);//!< Color to be used for drawing the histogram
    int bins_;                                   //!< Number of bins
    cv::Mat mat_;                                //!< cv:Matrix containing the histogram data
    explicit Histogram(int bins);
};

/**
 * @brief The Image class provides methods for manipulating digital images.
 */
class Image {
    cv::Mat image_mat_;
    COLOR_SPACE c_space = BGR;
    CHROMA_SUBSAMPLING cs_ratio = NA;

public:
    Image() = default;
    ~Image() = default;
    //! Create an Image from an open BGR cv::Mat
    explicit Image(const cv::Mat &arr2d);

    //! Create an Image from a given file path
    explicit Image(const char *filename);

    //! Loads an Image from a cv::Mat
    //! @return Image file
    Image *load(const cv::Mat &arr2d);

    //! @brief Sets the underlying cv::Mat
    void set_image_mat(cv::Mat mat);

    //! @brief Returns the underlying cv::Mat
    cv::Mat *get_image_mat();

    //! @brief Sets the color space of the Image
    void set_color(COLOR_SPACE col);

    //! @brief Returns the color space of the Image
    COLOR_SPACE get_color() const;

    //! @brief Sets the chroma subsampling rate of the Image
    void set_chroma(CHROMA_SUBSAMPLING cs);
    //! @brief Returns the chroma subsampling rate of the Image
    CHROMA_SUBSAMPLING get_chroma() const;

    //! Used to iterate over all pixels in the Image
    //! \warning Unless you need to iterate from first to last pixel in order, use cv:Mat::forEach instead
    //! @return Iterator to the first pixel in the Image
    cv::MatIterator_<cv::Vec3b> begin();

    //! Used to iterate over all pixels in the Image
    //! \warning Unless you need to iterate from first to last pixel in order, use cv:Mat::forEach instead
    //! @return Iterator to the last pixel in the Image
    cv::MatIterator_<cv::Vec3b> end();

    //! Return size of Image
    //! @return Size of Image as a cv::Size object
    cv::Size size() const;

    //! Loads an Image from a file
    //! \details The Image is stored in a 8-bit unsigned integer matrix with 3
    //! channels (BGR)
    //! @param  filename Absolute path to Image file
    //! @param  mode Mode to load Image in (see cv:ImreadModes for available
    //! modes)
    void load(const char *filename, cv::ImreadModes mode = cv::IMREAD_COLOR);

    //! Saves an Image to a file
    //! @param  filename Absolute path to Image file
    //! @param  compression_params Vector of integers specifying the compression
    //! parameters with format <paramID, paramValue> (see cv:ImwriteFlags for
    //! available parameters)
    void save(const char *filename, const std::vector<int> &compression_params = {}) const;

    //! Get data type of Image
    //! @return Integer indicating the cv:Mat type of the underlying matrix
    int getImageType() const;

    //! Displays the Image in a window
    //! @params vid_ctx Indicates whether it is to be used in displaying a video (
    void show(bool vid_ctx = false) const;

    //! Returns whether the Image has been loaded
    //! @return Boolean indicating whether the Image has been loaded
    bool loaded() const;

    //! Get color values from a pixel (0-255 range)
    //! @param  row Row of pixel / y-coordinate
    //! @param  col Column of pixel / x-coordinate
    //! @return Array of integers containing the color values of the pixel in the
    cv::Vec3b getPixel(int row, int col) const;

    //! Set color values of a pixel (0-255 range)
    //! @param  row Row of pixel / y-coordinate
    //! @param  col Column of pixel / x-coordinate
    //! @param  color_values Array of integers containing the color values of the
    //! pixel
    void setPixel(int row, int col, const cv::Vec3b &color_values);

    //! Get a deep copy of the Image
    //! @return Image object containing a deep copy of the Image
    Image clone() const;

    //! Equality operator
    //! @param  other Image to be compared to
    //! @return Boolean indicating whether the two images are equal
    bool operator==(Image &other) const;

    //! Create color histograms of a BGR Image
    //! @param  bins Number of bins in the histogram
    //! @param  fill_hist Indicates whether the histogram should be filled
    //! @param  width Width in pixels of the histogram
    //! @param  height Height in pixels of the histogram
    //! @return Vector with resulting Histograms
    std::vector<cv::Mat> color_histograms(int bins = 256, bool fill_hist = false, int width = 512,
                                          int height = 400) const;

    //! Apply the gaussian blur filter to this image
    //! @param blur Blur coefficient matrix (values are int)
    //! @return Copy of image with the blur applied
    Image gaussian_blur(cv::Mat blur);

    //! Returns a square slice of the image matrix
    //! @param  row Row of the first pixel
    //! @param  col Column of the first pixel
    //! @param  size Size of the square
    //! @return Matrix containing the slice
    cv::Mat getSlice(int row, int col, int size) const;

    //! Returns a copy of a portion of the image matrix (values out of range are excluded)
    //! @param radiusR vertical radius
    //! @param radiusC horizontal radius
    //! @param r,c the coordinates of the central pixel
    //! @return The selected submatrix
    cv::Mat get_neighbors(int radiusR, int radiusC, int r, int c) const;

    //! Cuts the given matrix so that, if the center of that matrix were to be placed at the given coordinates in image_mat_, it would not overflow
    //! @param m the given matrix
    //! @param row row of the coordinates
    //! @param col column of the coordinates
    //! @return The cut submatrix
    cv::Mat cut(const cv::Mat &m, int row, int col) const;
};

/**
 * \brief Inserts a matrix into another matrix
 * \details The function alters the mat matrix, inserting the slice matrix using the row and col parameters as the top left corner of the insertion
 * \param mat Matrix to alter
 * \param slice Matrix to insert
 * \param row Row of insertion
 * \param col Column of insertion
 */
void setSlice(const cv::Mat &mat, const cv::Mat &slice, int row, int col);

//! @brief Creates basic histogram of matrix
//! @details The functions needs to be called with the correct template type,
//! according to the type of the matrix
//! @param  matrix Matrix with data
//! @param  bins Number of bins (probably use the same number as the number of
//! possible values in the matrix)
//! @return Histogram of matrix
template<typename T>
cv::Mat histogram(const cv::Mat &matrix, const int bins = 256) {
    cv::Mat histValues = cv::Mat::zeros(1, bins, CV_32F);
    matrix.forEach<T>([&](T &pixel, const int position[]) -> void { histValues.at<float>(pixel) += 1; });
    return histValues;
}