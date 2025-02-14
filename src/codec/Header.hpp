/**
 * @file Header.hpp
 * @brief Header class
 * @ingroup Codec
 * Contains the Header class, which defines the basic header structure for the codec, and the InterHeader and HybridHeader classes, which define the headers for the inter and hybrid codecs, respectively.
 */

#pragma once
#include "../visual/Image.hpp"
#include "../visual/YuvHeader.hpp"

class Frame;

/**
 * \brief The Header class defines the basic header structure for the codec
 */
class Header {
public:
    COLOR_SPACE color_space;              //!< Color space
    CHROMA_SUBSAMPLING chroma_subsampling;//!< Chroma subsampling format
    uint32_t width;                       //!< Width
    uint32_t height;                      //!< Height
    uint8_t golomb_m;                     //!< Golomb m parameter
    uint32_t length;                      //!< Number of frames
    uint8_t fps_num;                      //!< FPS numerator
    uint8_t fps_den;                      //!< FPS denominator
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
     * \param bs BitStream reference
     */
    void write_header(BitStream &bs) const;
    /**
     * \brief Extracts header info from given Frame
     * \details Extracts color space, chroma subsampling format, width and height from given Frame
     * \param frame Frame to extract info from
     */
    void extract_info(const Frame &frame);
    /**
     * \brief Reads header from BitStream
     * \param bs BitStream pointer
     * \return Header object
     */
    Header static read_header(BitStream &bs);
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
    explicit InterHeader(const Header &header);
    /**
     * \brief Writes header to BitStream
     * \param bs BitStream reference
     */
    void write_header(BitStream &bs) const;
    /**
     * \brief Reads header from BitStream
     * \param bs BitStream pointer
     * \return InterHeader object
     */
    static InterHeader read_header(BitStream &bs);
};