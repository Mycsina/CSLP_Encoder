//!  @file Declares functions that change the data of an image
/*!
 * @brief Declares functions that change the data of an image
 */

#pragma once

#include "image.hpp"

using namespace std;
using namespace cv;

//! Inserts a watermark into an image
/*!
 * Inserts a watermark into an image
 * @param im The image to be watermarked
 * @param mark The watermark image
 * @param coord1 The top left corner of the watermark
 * @param coord2 The bottom right corner of the watermark
 * @param alpha The opacity of the watermark (1.0 = transparent, 0.0 = opaque)
 */
void watermark(image im, image *mark, Point2i coord1, Point2i coord2,
               double alpha);

//! Convert an image from BGR to YUV color space (as explained in wikipedia)
//! @param im Image to be converted
void BGR2YUV(image im);

//! Convert an image from YUV to BGR color space (as explained in wikipedia)
//! @param im Image to be converted
void YUV2BGR(image im);

//! Convert an image from BGR to GRAY color space (using formula for luma)
//! @param im Image to be converted
void subsample(image im, CHROMA_SUBSAMPLING cs);
