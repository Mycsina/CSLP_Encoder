/**
    * @file LosslessHybrid.hpp
    * @brief LosslessHybrid encoder class
    * @ingroup codec, lossless, encoders
    */

#pragma once
#include "../../Encoder.hpp"
#include "../../Header.hpp"

class HybridHeader : public InterHeader {
public:
    uint8_t period{};       //!< Period of intra frames
    uint8_t search_radius{};//!< Search radius

    /**
     * \brief Default constructor
     */
    HybridHeader() = default;
    /**
     * \brief Constructor
     * \param header Base header to copy from
     */
    explicit HybridHeader(const Header &header);
    /**
     * \brief Writes header to BitStream
     * \param bs BitStream pointer
     */
    void write_header(BitStream &bs) const;
    /**
     * \brief Reads header from BitStream
     * \param bs BitStream pointer
     * \return HybridHeader object
     */
    static HybridHeader read_header(BitStream &bs);
};

/**
 * \brief The LosslessHybridEncoder class provides methods to encode/decode video using the lossless hybrid codec
 */
class LosslessHybridEncoder final : public Encoder {
public:
    const char *src{};      ///< File path of the input video
    const char *dst{};      ///< File path of the encoded video
    HybridHeader header{};  ///< Header object
    uint8_t golomb_m;       ///< Golomb m parameter
    uint8_t block_size;     ///< Macroblock size
    uint8_t period{};       ///< Period of intra frames
    int sample_factor = 100;///< Sample factor for calculating golomb_m

    /**
     * \brief Constructor for the LosslessHybridEncoder class
     * \param src File path of the input video
     * \param dst File path of the encoded video
     * \param golomb_m Golomb m parameter
     * \param block_size Macroblock size
     * \param period Period of intra frames
     */
    LosslessHybridEncoder(const char *src, const char *dst, uint8_t golomb_m, uint8_t block_size, uint8_t period);
    /**
     * \brief Constructor for the LosslessHybridEncoder class
     * \param src File path of the input encoded video
     * \param dst File path of the decoded video
     */
    LosslessHybridEncoder(const char *src, const char *dst);
    /**
     * \brief Constructor for the LosslessHybridEncoder class
     * \param src File path of the input encoded video
     */
    explicit LosslessHybridEncoder(const char *src);
    LosslessHybridEncoder() = default;

    void encode() override;
    void decode() override;
};