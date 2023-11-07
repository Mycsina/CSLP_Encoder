//! @file This file contains the implementation of the classes and structs used in motion encoding
#pragma once

#include "../visual/Image.hpp"
#include <opencv2/core/mat.hpp>
#include <ostream>

struct MotionVector {
    int x, y;
    cv::Mat residual;
    MotionVector();
    MotionVector(int x, int y);
    friend std::ostream &operator<<(std::ostream &os, const MotionVector &vector);
};
class Frame;
//! @brief The Block class represents a block of pixels.
class Block {
private:
    cv::Mat block_mat_;
    int size_;
    int row_, col_;

public:
    Block(const Image &img, int size, int row, int col);

    const cv::Mat &getBlockMat() const;
    void setBlockMat(const cv::Mat &blockMat);
    int getSize() const;
    int getRow() const;
    int getCol() const;
    bool isLeftEdge() const;
    //! Returns the the block's vertices
    //! @return Array of integers representing the block's vertices in the format [x1, y1, x2, y2]
    std::array<int, 4> getVertices() const;

    class BlockDiff {
    public:
        virtual double block_diff(const Block &a, const Block &b) = 0;
        virtual bool isBetter(double score) = 0;
        double best_score{};
        MotionVector best_match;
        MotionVector previous_best;
        int threshold{};
        virtual void reset();
        bool compare(const Block &block, Frame *reference, cv::Point center);
    };

    class MAD : public BlockDiff {
    public:
        //! Returns the [MAD](https://en.wikipedia.org/wiki/Mean_absolute_difference) between this block and another block
        //! @details Higher MAD values indicate a greater difference between the blocks
        //! @param other Block to be compared
        //! @return MAL value (lesser is better)
        explicit MAD(int threshold = 0);
        double block_diff(const Block &a, const Block &b) override;
        bool isBetter(double score) override;
    };

    class MSE : public BlockDiff {
    public:
        //! Returns the [MSE](https://en.wikipedia.org/wiki/Mean_squared_error) between this block and another block
        //! @details Higher MSE values indicate a greater difference between the blocks
        //! @param other Block to be compared
        //! @return MSE value (lesser is better)
        explicit MSE(int threshold = 0);
        double block_diff(const Block &a, const Block &b) override;
        bool isBetter(double score) override;
    };

    class PSNR : public BlockDiff {
    public:
        //! Returns the [PSNR](https://en.wikipedia.org/wiki/Peak_signal-to-noise_ratio) between this block and another block
        //! @details Lower PSNR values indicate a greater difference between the blocks (logarithmic scale)
        //! @param other Block to be compared
        //! @return PSNR value (greater is better)
        explicit PSNR(int threshold = 0);
        double block_diff(const Block &a, const Block &b) override;
        bool isBetter(double score) override;
        void reset() override;
    };

    class SAD : public BlockDiff {
    public:
        //! Returns the [SAD](https://en.wikipedia.org/wiki/Sum_of_absolute_differences) between this block and another block
        //! @details Higher SAD values indicate a greater difference between the blocks
        //! @note This is the default block_diff method and is the fastest
        //! @param other Block to be compared
        //! @return SAD value (lesser is better)
        explicit SAD(int threshold = 0);
        double block_diff(const Block &a, const Block &b) override;
        bool isBetter(double score) override;
    };
};

//! Return a Block object representing a block of pixels
//! @param size Size of the block
//! @param row Row of the top-left pixelp
//! @param col Column of the top-left pixel
Block get_block(const Image &img, int size, int row, int col);


enum FrameType {
    I_FRAME,//!< Intra-frame
    P_FRAME,//!< Predicted frame
    B_FRAME //!< Bi-directionally predicted frame
};

/**
 * @brief The Frame class provides methods to manipulate an Image in the context of video encoding
 */
class Frame {
private:
    Image image_;      //!< Contains original image
    FrameType type_{}; //!< Indicates the type of frame
    cv::Mat frame_mat_;//!< Contains transformed image matrix
    Block::BlockDiff *block_diff_{};
    std::vector<MotionVector> motion_vectors_;

public:
    Frame() = default;
    ~Frame() = default;
    explicit Frame(Image img);
    Image getImage() const;
    cv::Mat *getFrameMat();
    void setFrameMat(const cv::Mat &frameMat);
    bool isBlockDiff(Block::BlockDiff *blockDiff) const;
    void setBlockDiff(Block::BlockDiff *blockDiff);
    std::vector<MotionVector> getMotionVectors() const;
    FrameType getType() const;
    void setType(FrameType type);
    void display_frame();
    void display_frame_original();

    void encode_JPEG_LS();

    static Image decode_JPEG_LS(const std::string &path);

    static uchar predict_JPEG_LS(cv::Mat mat, int row, int col);

    //! Returns a valid search window
    //! @param block Block that is being compared (top-left corner)
    //! @param search_radius Radius of the search area (around the block itself)
    //! @return Array of integers representing the search window in the format [x1, y1, x2, y2]
    std::array<int, 4> get_search_window(const Block &block, int search_radius) const;

    std::array<cv::Point, 5> get_rood_points(cv::Point center, int arm_size, int block_size) const;


    //! Returns the best motion vector between this frame and the nth previous frame
    //! @details This function uses an optimized version of [Exhaustive Search](https://en.wikipedia.org/wiki/Block-matching_algorithm#Exhaustive_Search), checking the block at it's original position first.
    //! @param block Block to be compared
    //! @param n Number of frames to go back
    //! @param search_radius Radius of the search area (not including the block itself)
    //! @return Motion vector
    MotionVector match_block_es(const Block &block, Frame *reference, int search_radius);

    //! Returns the motion vector between this frame and the nth previous frame
    //! @param block Block to be compared
    //! @param n Number of frames to go back
    //! @param search_radius Radius of the search area (not including the block itself)
    //! @return Motion vector
    MotionVector match_block_arps(const Block &block, Frame *reference, int threshold = 512);

    //! Calculate motion vectors for all blocks in the frame
    //! @param block_size Size of the macroblocks to be compared
    //! @param n Number of frames to go back
    //! @param search_radius Radius of the search area (not including the block itself)
    //! @param fast Indicates whether the fast search algorithm should be used
    //! @return Vector of motion vectors
    void calculate_MV(int block_size, Frame *reference, int search_radius, bool fast);

    // TODO
    //! Reconstruct a frame using a frame and a vector of motion vectors
    //! @param reference Reference frame
    //! @param motion_vectors Vector of motion vectors
    //! @return Reconstructed frame
    Frame reconstruct_frame(Frame *reference, const std::vector<MotionVector> &motion_vectors);
};