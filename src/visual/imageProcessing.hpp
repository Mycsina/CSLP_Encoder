//!  @file Declares functions that change the data of an Image
/*!
 * @brief Declares functions that change the data of an Image
 */

#pragma once
#include "Image.hpp"

using namespace std;
using namespace cv;

//! Inserts a watermark into an Image
/*!
 * Inserts a watermark into an Image
 * @param im The Image to be watermarked
 * @param mark The watermark Image
 * @param coord1 The top left corner of the watermark
 * @param coord2 The bottom right corner of the watermark
 * @param alpha The opacity of the watermark (1.0 = transparent, 0.0 = opaque)
 */
void watermark(Image im, Image *mark, Point2i coord1, Point2i coord2,
               double alpha);

//! Convert an Image from BGR to YUV color space (as explained in wikipedia)
//! @param im Image to be converted
void BGR2YUV(Image *im);

//! Convert an Image from YUV to BGR color space (as explained in wikipedia)
//! @param im Image to be converted
void YUV2BGR(Image *im);

//! Convert an Image from BGR to GRAY color space (using formula for luma)
//! @param im Image to be converted
void BGR2GRAY(Image *im);

//! Convert an Image from a higher YUV subsampling to a lower one
//! @param im Image to be converted
//! @param cs Chroma subsampling format
void subsample(Image *im, CHROMA_SUBSAMPLING cs);

//! Equalize the color histogram of an Image
//! @param im Image to be equalized
void equalize_hist(Image *im);