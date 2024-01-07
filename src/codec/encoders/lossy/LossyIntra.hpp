#pragma once

#include "../../Encoder.hpp"
#include "../../Header.hpp"
#include "../../Quantizer.hpp"

class LossyIntraHeader : public Header {
public:
    uint8_t y{};            //!< Quantization steps for Y channel
    uint8_t u{};            //!< Quantization steps for U channel
    uint8_t v{};            //!< Quantization steps for V channel
    /**
     * \brief Default constructor
     */
    LossyIntraHeader() = default;
    /**
     * \brief Constructor
     * \param header Base header to copy from
     */
    explicit LossyIntraHeader(const Header &header);
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
    static LossyIntraHeader read_header(BitStream &bs);
};

class LossyIntraEncoder final : public Encoder {
public:
    const char *src{};         ///< File path of the input video
    const char *dst{};         ///< File path of the encoded video
    LossyIntraHeader header{};///< Header object
    uint8_t golomb_m;          ///< Golomb m parameter
    uint8_t y = 0;             ///< Quantization steps for Y channel
    uint8_t u = 0;             ///< Quantization steps for U channel
    uint8_t v = 0;             ///< Quantization steps for V channel
    Quantizer y_quant;     ///< Quantizer for Y channel
    Quantizer u_quant;     ///< Quantizer for U channel
    Quantizer v_quant;     ///< Quantizer for V channel

    LossyIntraEncoder(const char *src, const char *dst, uint8_t golomb_m, uint8_t y, uint8_t u, uint8_t v);
    explicit LossyIntraEncoder(const char *src);
    /**
     * \brief encodes a video from src into dst with a mix of inter and intraframe encoding
     */
    void encode() override;
    /**
     * \brief decodes a video from dst with a mix of inter and intraframe encoding
     */
    void decode() override;

    void encode_JPEG_LS(Frame &frame) const;

    Frame decode_intra(Golomb &g) const;
};
