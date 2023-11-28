#pragma once

#include "../Encoder.hpp"
#include "../Header.hpp"

class LossyHybridEncoder final : public Encoder{
private:
    const char *src{};    ///< File path of the input video
    const char *dst{};    ///< File path of the encoded video
    HybridHeader header{};///< Header object
    uint8_t golomb_m;     ///< Golomb m parameter
    uint8_t block_size;   ///< Macroblock size
    uint8_t period{};     ///< Period of intra frames
    uint8_t fps;          ///< Frames per second
    uint8_t quant_bits;   ///< Number of bits in quantization

public:
    LossyHybridEncoder(const char *src, const char *dst, const uint8_t golomb_m, const uint8_t block_size, uint8_t period, uint8_t quant_bits);
    /**
     * \brief encodes a video from src into dst with a mix of inter and intraframe encoding
     */
    void encode() override;
    /**
     * \brief decodes a video from dst with a mix of inter and intraframe encoding
     */
    void decode() override;

    Frame decode_intra(Golomb *g, Header header);
};
