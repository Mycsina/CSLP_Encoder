/**
    * @file LosslessHybrid.hpp
    * @brief LosslessHybrid encoder class
    */

#pragma once
#include "../Encoder.hpp"
#include "../Header.hpp"

/**
 * \brief The LosslessHybridEncoder class provides methods to encode/decode video using the lossless hybrid codec
 */
class LosslessHybridEncoder final : public Encoder {
public:
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
     * \param src File path of the input video
     * \param dst File path of the encoded video
     */
    LosslessHybridEncoder(const char *src, const char *dst);
    const char *src{};    ///< File path of the input video
    const char *dst{};    ///< File path of the encoded video
    HybridHeader header{};///< Header object
    uint8_t golomb_m;     ///< Golomb m parameter
    uint8_t block_size;   ///< Macroblock size
    uint8_t period{};     ///< Period of intra frames
    uint8_t fps;          ///< Frames per second
    void encode() override;
    void decode() override;
};