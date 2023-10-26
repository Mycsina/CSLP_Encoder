//! @file This file contains the implementation of the classes and structs used in motion encoding
#pragma once

#include "../visual/Image.hpp"
#include <opencv2/core/mat.hpp>
#include <ostream>

struct MotionVector {
    int x, y;
    MotionVector();
    MotionVector(int x, int y);
    friend std::ostream &operator<<(std::ostream &os, const MotionVector &vector);
};

//! @brief The Block class represents a block of pixels.
class Blocks {
private:
    Image parent_;
    cv::Mat block_mat_;
    int size_;
    int row_, col_;

public:
    Blocks(const Image &img, int size, int row, int col) {
        parent_ = img;
        size_ = size;
        row_ = row;
        col_ = col;
        block_mat_ = img.get_slice(row, col, size);
    }

    const cv::Mat &getBlockMat() const {
        return block_mat_;
    }
    void setBlockMat(const cv::Mat &blockMat) {
        block_mat_ = blockMat;
    }
    int getSize() const {
        return size_;
    }
    int getRow() const {
        return row_;
    }
    int getCol() const {
        return col_;
    }

    //! Returns the MAD between this block and another block
    //! @param other Block to be compared
    //! @return MAL value (lesser is better)
    double compare_block_mal(const Blocks &other) const;

    //! Returns the MSE between this block and another block
    //! @param other Block to be compared
    //! @return MSE value (lesser is better)
    double compare_block_mse(const Blocks &other) const;

    //! Returns the [PSNR](https://en.wikipedia.org/wiki/Peak_signal-to-noise_ratio) between this block and another block
    //! @param other Block to be compared
    //! @return PSNR value (greater is better)
    double compare_block_psnr(const Blocks &other) const;
};

//! Return a Block object representing a block of pixels
//! @param size Size of the block
//! @param row Row of the top-left pixelp
//! @param col Column of the top-left pixel
Blocks get_block(const Image &img, int size, int row, int col);

/**
 * @brief The Frame class provides methods to manipulate an Image in the context of video encoding
 */
class Frame {
private:
    Image image_;
    cv::Mat frame_mat_;
    Frame *previous_{};
    Frame *next_{};// Allows for bidirectional traversal of the frames
public:
    Frame() = default;
    ~Frame() = default;
    explicit Frame(Image img);
    Image getImage() const;
    cv::Mat *getFrameMat();
    void setFrameMat(const cv::Mat &frameMat);
    Frame *getPrevious() const;
    void setPrevious(Frame *previous);
    Frame *getNext() const;
    void setNext(Frame *next);
    void display_frame();
    void display_frame_original();
    cv::Mat get_difference();

    //! Returns the best motion vector between this frame and the nth previous frame
    //! @details This function uses an optimized version of [Exhaustive Search](https://en.wikipedia.org/wiki/Block-matching_algorithm#Exhaustive_Search), checking the block at it's original position first.
    //! @param block Block to be compared
    //! @param n Number of frames to go back
    //! @param search_radius Radius of the search area (not including the block itself)
    //! @return Motion vector
    MotionVector match_block_es(const Blocks &block, Frame *reference, int search_radius);

    //! Returns the motion vector between this frame and the nth previous frame
    //! @param block Block to be compared
    //! @param n Number of frames to go back
    //! @param search_radius Radius of the search area (not including the block itself)
    //! @return Motion vector
    MotionVector match_block_arps(const Blocks &block, Frame *reference, int search_radius);

    //! Returns all motion vectors between this frame and the nth previous frame
    //! @param block_size Size of the macroblocks to be compared
    //! @param n Number of frames to go back
    //! @param search_radius Radius of the search area (not including the block itself)
    //! @param fast Indicates whether the fast search algorithm should be used
    //! @return Vector of motion vectors
    std::vector<MotionVector> match_all_blocks(int block_size = 16, int n = 1, int search_radius = 7, bool fast = true);

    //! Reconstruct a frame using a reference frame and a vector of motion vectors
    //! @param reference Reference frame
    //! @param motion_vectors Vector of motion vectors
    //! @return Reconstructed frame
    Frame reconstruct_frame(Frame *reference, const std::vector<MotionVector> &motion_vectors);
};
