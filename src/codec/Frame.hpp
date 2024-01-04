/** @file Frame.hpp
 * @brief The Frame class provides methods to manipulate an Image in the context of video encoding
 * @ingroup codec
 */

#pragma once

#include "../visual/Image.hpp"
#include "Header.hpp"
#include <opencv2/core/mat.hpp>

//! @brief The MotionVector struct represents a motion vector and its residual
struct MotionVector {
    int x, y;
    cv::Mat residual;
    MotionVector();
    MotionVector(int x, int y);
    friend std::ostream &operator<<(std::ostream &os, const MotionVector &vector);
    bool operator==(const MotionVector &rhs) const;
};
class Frame;
//! @brief The Block class represents a block of pixels.
class Block {
    cv::Mat block_mat_;//!< Contains the block's pixels
    int size_;         //!< Size of the block
    int row_, col_;    //!< Position of the block's top-left corner

public:
    /**
     * \brief Constructor from Image
     * \param img Image to copy from
     * \param size Size of the block
     * \param row Row of the top-left pixel
     * \param col Column of the top-left pixel
     */
    Block(const Image &img, int size, int row, int col);

    /**
     * \brief Gets the block's matrix
     * \return Const reference to the block's matrix
     */
    const cv::Mat &getBlockMat() const;
    /**
     * \brief Sets the block's matrix
     * \param blockMat Block matrix to set
     */
    void setBlockMat(const cv::Mat &blockMat);
    /**
     * \brief Gets the block's size
     * \return Block's size
     */
    int getSize() const;
    /**
     * \brief Gets the block's top-left corner's row
     * \return Block's row
     */
    int getRow() const;
    /**
     * \brief Gets the block's top-left corner's column
     * \return Block's column
     */
    int getCol() const;
    /**
     * \brief Checks if block is in first column
     * \return Boolean indicating whether the block is in the first column
     */
    bool isLeftEdge() const;

    //! \brief Returns the block's vertices
    //! @return Array of integers representing the block's vertices in the format [x1, y1, x2, y2]
    std::array<int, 4> getVertices() const;

    /**
     * \brief Block difference abstract class
     */
    class BlockDiff {
    public:
        /**
         * \brief Destructor
         */
        virtual ~BlockDiff() = default;
        /**
         * \brief Returns the difference between a block and another block
         * \param a Block to be compared
         * \param b Other block to compare
         * \return Double representing the difference between the blocks
         */
        virtual double block_diff(const Block &a, const Block &b) = 0;
        /**
         * \brief Checks if a score is better than the best score
         * \param score Score to be compared
         * \return Boolean indicating whether the score is better than the best score
         */
        virtual bool isBetter(double score) = 0;
        double best_score{};       //!< Best score
        MotionVector best_match;   //!< Best motion vector
        int threshold{};           //!< Threshold for the block difference
        virtual void reset();      //!< Resets the best score and best motion vector
        /**
         * \brief Compares a block to a reference frame
         * \param block Block to be compared
         * \param reference Reference frame/search space
         * \param center Where the block will be placed to be compared
         * \return Boolean indicating whether search is finished (score is below threshold)
         */
        bool compare(const Block &block, const Frame *reference, cv::Point center);
    };

    class MAD final : public BlockDiff {
    public:
        explicit MAD(int threshold = 0);

        //! Returns the [MAD](https://en.wikipedia.org/wiki/Mean_absolute_difference) between a block and another block
        //! @details Higher MAD values indicate a greater difference between the blocks
        //! @param a Block to be compared
        //! @param b Block to be compared
        //! @return MAL value (lesser is better)
        double block_diff(const Block &a, const Block &b) override;
        bool isBetter(double score) override;
    };

    class MSE final : public BlockDiff {
    public:
        explicit MSE(int threshold = 0);

        //! Returns the [MSE](https://en.wikipedia.org/wiki/Mean_squared_error) between this block and another block
        //! @details Higher MSE values indicate a greater difference between the blocks
        //! @param a Block to be compared
        //! @param b Block to be compared
        //! @return MSE value (lesser is better)
        double block_diff(const Block &a, const Block &b) override;
        bool isBetter(double score) override;
        int threshold_;
    };

    class PSNR final : public BlockDiff {
    public:
        explicit PSNR(int threshold = 0);

        //! Returns the [PSNR](https://en.wikipedia.org/wiki/Peak_signal-to-noise_ratio) between this block and another block
        //! @details Lower PSNR values indicate a greater difference between the blocks (logarithmic scale)
        //! @param a Block to be compared
        //! @param b Block to be compared
        //! @return PSNR value (greater is better)
        double block_diff(const Block &a, const Block &b) override;
        bool isBetter(double score) override;
        void reset() override;
    };

    class SAD final : public BlockDiff {
    public:
        explicit SAD(int threshold = 0);

        //! Returns the [SAD](https://en.wikipedia.org/wiki/Sum_of_absolute_differences) between this block and another block
        //! @details Higher SAD values indicate a greater difference between the blocks
        //! @note This is the default block_diff method and is the fastest
        //! @param a Block to be compared
        //! @param b Block to be compared
        //! @return SAD value (lesser is better)
        double block_diff(const Block &a, const Block &b) override;
        bool isBetter(double score) override;
    };
};

//! Returns a Block object representing a block of pixels
//! @param img Image to be used
//! @param size Size of the block
//! @param row Row of the top-left pixelp
//! @param col Column of the top-left pixel
//! @return Block object
Block get_block(const Image &img, int size, int row, int col);


/**
 * \brief Frame type enum
 */
enum FrameType {
    I_FRAME,//!< Intra-frame
    P_FRAME,//!< Predicted frame
    B_FRAME //!< Bi-directionally predicted frame
};

/**
 * @brief The Frame class provides methods to manipulate an Image in the context of video encoding
 */
class Frame {
    Image image_;                             //!< Contains original image
    FrameType type_{};                        //!< Indicates the type of frame
    Block::BlockDiff *block_diff_{};          //!< Block difference method
    std::vector<MotionVector> motion_vectors_;//!< Vector of motion vectors
    std::vector<int> intra_encoding;          //!< Vector of intra encoding values

public:
    /**
     * \brief Default constructor
     */
    Frame() = default;
    /**
     * \brief Default destructor
     */
    ~Frame() = default;
    /**
     * \brief Constructor from Image
     * \param img Image to copy from
     */
    explicit Frame(const Image &img);
    /**
     * \brief Returns the Image object
     * \return Image object
     */
    Image get_image() const;
    /**
     * \brief Checks if blockDiff method has been set
     * \param blockDiff Block difference method
     * \return Boolean indicating whether given blockDiff method has been set
     */
    bool is_block_diff(const Block::BlockDiff *blockDiff) const;
    /**
     * \brief Sets the blockDiff method
     * \param blockDiff Block difference method
     */
    void set_block_diff(Block::BlockDiff *blockDiff);
    /**
     * \brief Returns the calculated motion vectors
     */
    std::vector<MotionVector> get_motion_vectors() const;
    /**
     * \brief Returns the calculated intra encoding values
     */
    const std::vector<int> &get_intra_encoding() const;
    /**
     * \brief Gets the type of frame
     */
    FrameType get_type() const;
    /**
     * \brief Sets the type of frame
     */
    void setType(FrameType type);
    /**
     * \brief Displays the frame in a window
     */
    void show();

    void encode_JPEG_LS();

    void encode_JPEG_LS(const Golomb &g);

    void write_JPEG_LS(Golomb &g) const;

    static Frame decode_JPEG_LS(Golomb &g, const Header &header);

    static Frame decode_JPEG_LS(const std::vector<int> &encodings, COLOR_SPACE color, CHROMA_SUBSAMPLING chroma, int rows, int cols);

    static uchar predict_JPEG_LS(cv::Mat mat, int row, int col, int channel = 0);

    //! Returns a valid search window
    //! @param block Block that is being compared (top-left corner)
    //! @param search_radius Radius of the search area (around the block itself)
    //! @return Array of integers representing the search window in the format [x1, y1, x2, y2]
    std::array<int, 4> get_search_window(const Block &block, int search_radius) const;

    std::vector<cv::Point> get_rood_points(cv::Point center, int arm_size, int block_size) const;

    //! Returns the best motion vector between this frame and the nth previous frame
    //! @details This function uses an optimized version of [Exhaustive Search](https://en.wikipedia.org/wiki/Block-matching_algorithm#Exhaustive_Search), checking the block at it's original position first.
    //! @param block Block to be compared
    //! @param reference Reference frame
    //! @param search_radius Radius of the search area (not including the block itself)
    //! @return Motion vector
    MotionVector match_block_es(const Block &block, const Frame *reference, int search_radius) const;

    //! Returns the motion vector between this frame and the nth previous frame
    //! @details This function uses the [Adaptive Rood Pattern Search](https://ieeexplore.ieee.org/document/1176932) algorithm
    //! @param block Block to be compared
    //! @param reference Reference frame
    //! @return Motion vector
    MotionVector match_block_arps(const Block &block, Frame *reference) const;

    //! Calculate motion vectors for all blocks in the frame
    //! @param block_size Size of the macroblocks to be compared
    //! @param reference Reference frame
    //! @param search_radius Radius of the search area (not including the block itself)
    //! @param fast Indicates whether the fast search algorithm should be used
    //! @return Vector of motion vectors
    void calculate_MV(Frame *reference, int block_size, int search_radius, bool fast);

    void visualize_MV(const Frame *reference, int block_size) const;

    //! Reconstruct a frame using a frame, a vector of motion vectors and a block size
    //! @param reference Reference frame
    //! @param motion_vectors Vector of motion vectors
    //! @param block_size Size of the macroblocks to be compared
    //! @return Reconstructed frame
    Frame static reconstruct_frame(Frame &reference, const std::vector<MotionVector> &motion_vectors, int block_size);

    //! Write the motions vectors of a frame to file using golomb encoding
    //! @param g reference to the Golomb encoder
    void write(const Golomb &g) const;

    //! Decodes a frame using interframe codec
    //! @param g refernece to the golomb encoder
    //! @param reference intraframe that serves as reference
    //! @param header header data
    //! @return decoded frame
    static Frame decode_inter(Golomb &g, Frame &reference, const InterHeader &header);
};