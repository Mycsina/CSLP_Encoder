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

// different YUVs are handled differently
enum COLOR_FORMAT {
    BGR,
    YUV444,
    YUV422,
    YUV420
};

class image
{
private:
    Mat image_mat_;
    COLOR_FORMAT format=BGR;
public:

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

    void _set_image_mat(Mat mat) { image_mat_=mat; }
    Mat _get_image_mat() { return image_mat_; }
    void _set_format(COLOR_FORMAT f){ format=f; }
    COLOR_FORMAT _get_format(){ return format; }



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
