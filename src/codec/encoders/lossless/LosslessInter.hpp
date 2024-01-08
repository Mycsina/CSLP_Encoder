/**
 * @file LosslessInter.hpp
 * @brief LosslessIntraFrameEncoder class
 */

#pragma once
#include "../../Encoder.hpp"
#include "../../Header.hpp"

/**
 * The LosslessInterFrameEncoder provides methods to encode/decode a video using the lossless interframe codec
 */
class LosslessInterFrameEncoder final : public Encoder {
public:
    /**
    * \brief Constructor for the LosslessInterFrameEncoder class
    * \param src File path of the input video
    * \param dst File path of the encoded video
    * \param golomb_m Golomb m parameter
    * \param block_size Macroblock size
    * \param period Period of intra frames
    */
    LosslessInterFrameEncoder(const char *src, const char *dst, uint8_t golomb_m, uint8_t block_size);
    /**
     * \brief Constructor for the LosslessInterFrameEncoder class
     * \param src File path of the input video
     * \param dst File path of the encoded video
     */
    LosslessInterFrameEncoder(const char *src, const char *dst);
    const char *src{};
    const char *dst{};
    InterHeader header{};
    uint8_t golomb_m;
    uint8_t block_size;
    /**
     * \brief Encodes a video from src into dst using interframe encoding
     */
    void encode() override;
    /**
     * \brief Decodes a video from dst using interframe encoding
     */
    void decode() override;
};