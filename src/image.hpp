//! @file image class declaration
/*!
 Declares the image class, representing a digital image.
*/
#pragma once
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>

using namespace std;
using namespace cv;

/* TODO
    Converting between color spaces.
    */

enum COLOR_FORMAT {
    BGR,
    YUV
};

class image
{
private:
    Mat image_mat_;
    COLOR_FORMAT format;
public:

    //! Used to get a pixel's BGR values
    //! @param x,y: the pixel's coordinates
    //! @return Vector with the BGR values (in that order)
    Vec3b get_BGR_values(int x, int y);

    //! Used to get a pixel's YUV values
    //! @param x,y: the pixel's coordinates
    //! @return Vector with the YUV values (in that order)
    Vec3f get_YUV_values(int x, int y);

    //! Used to convert BGR values to YUV values
    //! @param bgr: a matrix with the BGR values
    //! @return Vector with the YUV values
    Vec3f convert_BGR_YUV(Vec3b bgr);

    //! Used to convert YUV values to BGR values
    //! @param yuv: a matrix with the YUV values
    //! @return Vector with the BGR values
    Vec3b convert_YUV_BGR(Vec3f yuv);

    //! converts current image from BGR to YUV or vice-versa
    //! @return converted image (original image is not altered)
    image convert_image();

    Mat _set_image_mat(Mat mat) { image_mat_=mat; return image_mat_; }
    Mat _get_image_mat() { return image_mat_; }

    //! Used to iterate over all pixels in the image
    //! @return Iterator to the first pixel in the image
    MatIterator_<Vec3b> begin() { return image_mat_.begin<Vec3b>(); }

    //! Used to iterate over all pixels in the image
    //! @return Iterator to the last pixel in the image
    MatIterator_<Vec3b> end() { return image_mat_.end<Vec3b>(); }

    //! Loads an image from a cv::Mat
    //! @return Image file
    image* load(Mat *arr2d);

    //! Loads an image from a file
    //! \details The image is stored in a 8-bit unsigned integer matrix with 3 channels (BGR)
    //! @param  filename Absolute path to image file
    //! @param  mode Mode to load image in (see cv:ImreadModes for available modes)
    //! @return Boolean indicating whether image was loaded successfully
	void load(const char* filename, ImreadModes mode = IMREAD_COLOR);

    //! Saves an image to a file
    //! @param  filename Absolute path to image file
    //! @param  compression_params Vector of integers specifying the compression parameters with format <paramID, paramValue> (see cv:ImwriteFlags for available parameters)
    //! @return Boolean indicating whether image was saved successfully
    void save(const char* filename, const vector<int>& compression_params = {});

    //! Get size of image
    //! @return Array of integers containing the size of the image in the format <rows, cols>
    array<int, 2> get_image_size() const { return { image_mat_.rows, image_mat_.cols }; }

    //! Get data type of image
    //! @return Integer indicating the data type of the image
    int get_image_type() const { return image_mat_.type(); }

    //! Displays the image in a window
    //! @params vid_ctx Indicates whether it is to be used in displaying a video
    //! @return Void
	void display_image(bool vid_ctx = false);

    //! Returns whether the image has been loaded
    //! @return Boolean indicating whether the image has been loaded
	bool loaded() const { return !image_mat_.empty(); }

    //! Get color values from a pixel
    //! @param  row Row of pixel / y-coordinate
    //! @param  col Column of pixel / x-coordinate
    //! @return Array of integers containing the color values of the pixel in the format <B, G, R>
    Vec3b get_pixel(int row, int col) const;

    //! Set color values of a pixel
    //! @param  row Row of pixel / y-coordinate
    //! @param  col Column of pixel / x-coordinate
    //! @param  value Array of integers containing the color values of the pixel in the format <B, G, R>
    //! @return Void
    void set_pixel(int row, int col, Vec3b value);

    //! Get a deep copy of the image
    //! @return image object containing a deep copy of the image
    image clone();
};
