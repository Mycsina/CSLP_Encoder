/**
 * @file Header.hpp
 * @brief Header class
 * @ingroup Codec
 * Contains the Header class, which defines the basic header structure for the codec, and the InterHeader and HybridHeader classes, which define the headers for the inter and hybrid codecs, respectively.
 */

#pragma once
#include "../visual/Image.hpp"

class Frame;

/**
 * \brief The Header class defines the basic header structure for the codec
 */
class Header {
public:
    COLOR_SPACE color_space;              //!< Color space
    CHROMA_SUBSAMPLING chroma_subsampling;//!< Chroma subsampling format
    uint32_t width;                        //!< Width
    uint32_t height;                       //!< Height
    uint8_t golomb_m{};                   //!< Golomb m parameter
    uint32_t length{};                    //!< Number of frames
    /**
     * \brief Default constructor
     */
    Header() = default;
    /**
     * \brief Constructor
     * \param color_space Color space
     * \param cs Chroma subsampling format
     * \param width Width
     * \param height Height
     */
    Header(COLOR_SPACE color_space, CHROMA_SUBSAMPLING cs, uint8_t width, uint8_t height);
    /**
     * \brief Writes header to BitStream
     * \param bs BitStream pointer
     */
    void writeHeader(BitStream *bs) const;
    /**
     * \brief Extracts header info from given Frame
     * \details Extracts color space, chroma subsampling format, width and height from given Frame
     * \param frame Frame to extract info from
     */
    void extractInfo(const Frame &frame);
    /**
     * \brief Reads header from BitStream
     * \param bs BitStream pointer
     * \return Header object
     */
    Header static readHeader(BitStream *bs);
};

class InterHeader : public Header {
public:
    uint8_t block_size;//!< Block size
    /**
     * \brief Default constructor
     */
    InterHeader() = default;
    /**
     * \brief Constructor
     * \param header Base header to copy from
     */
    explicit InterHeader(Header header);
    /**
     * \brief Writes header to BitStream
     * \param bs BitStream pointer
     */
    void write_header(BitStream *bs) const;
    /**
     * \brief Reads header from BitStream
     * \param bs BitStream pointer
     * \return InterHeader object
     */
    static InterHeader readHeader(BitStream *bs);
};

class HybridHeader : public InterHeader {
public:
    uint8_t period;       //!< Period
    uint8_t search_radius;//!< Search radius
    uint8_t fps;          //!< FPS
    /**
     * \brief Default constructor
     */
    HybridHeader() = default;
    /**
     * \brief Constructor
     * \param header Base header to copy from
     */
    explicit HybridHeader(InterHeader header);
    /**
     * \brief Writes header to BitStream
     * \param bs BitStream pointer
     */
    void writeHeader(BitStream *bs) const;
    /**
     * \brief Reads header from BitStream
     * \param bs BitStream pointer
     * \return HybridHeader object
     */
    static HybridHeader readHeader(BitStream *bs);
};

class LossyHybridHeader: public HybridHeader{
public:
    uint8_t period;       //!< Period
    uint8_t search_radius;//!< Search radius
    uint8_t fps;          //!< FPS
    /**
     * \brief Default constructor
     */
    LossyHybridHeader() = default;
    /**
     * \brief Constructor
     * \param header Base header to copy from
     */
    explicit LossyHybridHeader(InterHeader header);
    /**
     * \brief Writes header to BitStream
     * \param bs BitStream pointer
     */
    void writeHeader(BitStream *bs) const;
    /**
     * \brief Reads header from BitStream
     * \param bs BitStream pointer
     * \return HybridHeader object
     */
    static HybridHeader readHeader(BitStream *bs);
};