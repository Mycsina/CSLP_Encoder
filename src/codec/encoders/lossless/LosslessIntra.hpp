/**
 * @file LosslessIntra.hpp
 * @brief LosslessIntraEncoder class
 * @ingroup codec, lossless, encoders
 */

#pragma once
#include "../../Encoder.hpp"
#include "../../Header.hpp"

/**
 * \brief The LosslessInterFrameEncoder provides methods to encode/decode a video using the lossless interframe codec
 */
class LosslessIntraEncoder final : public Encoder {
public:
    const char *src{};
    const char *dst{};
    uint8_t golomb_m = 0;
    Header header{};
    int sample_factor = 100;

    /**
     * \brief Constructor for the LosslessIntraFrameEncoder class
     * \param src File path of the input video
     * \param dst File path to write encoded video
     * \param golomb_m Golomb m parameter
     */
    LosslessIntraEncoder(const char *src, const char *dst, uint8_t golomb_m);
    /**
     * \brief Constructor for the LosslessIntraFrameEncoder class
     * \param src File path of the input encoded video
     * \param dst File path to write the decoded video
     */
    LosslessIntraEncoder(const char *src, const char *dst);
    /**
     * \brief Default constructor for the LosslessIntraFrameEncoder class
    *  \param src File path of the input encoded video
     */
    explicit LosslessIntraEncoder(const char *src);
    LosslessIntraEncoder() = default;

    void encode() override;
    void decode() override;
};