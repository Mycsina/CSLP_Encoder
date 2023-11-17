//!  @file Declares functions that change the data of an Image
/*!
 * @brief Declares functions that change the data of an Image
 */

#pragma once
#include "Image.hpp"

/*!
 * Inserts a watermark into an Image
 * @param im The Image to be watermarked
 * @param mark The watermark Image
 * @param coord1 The top left corner of the watermark
 * @param coord2 The bottom right corner of the watermark
 * @param alpha The transparency of the watermark (1.0 = transparent, 0.0 = opaque)
 */
void watermark(Image &im, Image mark, cv::Point2i coord1, cv::Point2i coord2,
               double alpha);

//! Converts Image from BGR to YUV 4:4:4
//! @return Image converted to YUV 4:4:4
Image convert_BGR_YUV444(Image &im);

//! Converts Image from BGR to YUV 4:2:2
//! @return Image converted to YUV 4:2:2
Image convert_BGR_YUV422(Image &im);

//! Converts Image from BGR to YUV 4:2:0
//! @return Image converted to YUV 4:2:0
Image convert_BGR_YUV420(Image &im);

//! Converts Image from YUV to BGR
//! @return Image converted from YUV to BGR
Image convert_YUV_BGR(Image &im);

//! Converts Image from BGR to GRAY
//! @return Image converted from BGR to GRAY
Image convert_BGR_GRAY(Image &im);

//! Convert an Image from a higher YUV subsampling to a lower one
//! @param im Image to be converted
//! @param cs Chroma subsampling format
void subsample(Image &im, CHROMA_SUBSAMPLING cs);

//! Equalize the color histogram of an Image
//! @param im Image to be equalized
void equalize_hist(Image &im);

//! Apply [Otsu's method](https://en.wikipedia.org/wiki/Otsu%27s_method#) to an Image
//! @param im Image to be binarized
void binarize(Image &im);