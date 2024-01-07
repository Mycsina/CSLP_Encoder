/**
 * @file LosslessIntra.hpp
 * @brief LosslessIntraEncoder class
 */

#pragma once
#include "../../Encoder.hpp"
#include "../../Header.hpp"

/**
 * \brief The LosslessInterFrameEncoder provides methods to encode/decode a video using the lossless interframe codec
 */
class LosslessIntraEncoder final : public Encoder {
public:
    /**
     * \brief Constructor for the LosslessIntraFrameEncoder class
     * \param src File path of the input video
     * \param dst File path of the encoded video
     */
    LosslessIntraEncoder(const char *src, const char *dst);
    /**
     * \brief Constructor for the LosslessIntraFrameEncoder class
     * \param src File path of the input video
     */
    explicit LosslessIntraEncoder(const char *src);

    const char *src{};
    const char *dst{};
    Header header{};
    int sample_factor = 100;
    /**
     * \brief encodes a video from src into dst using intraframe encoding
     */
    void encode() override;
    /**
     * \brief decodes a video from dst using intraframe encoding
     */
    void decode() override;
};