#pragma once

#include "../../Encoder.hpp"
#include "../../Header.hpp"
#include "../../Quantizer.hpp"

class LossyHybridHeader : public InterHeader {
public:
    uint8_t period{};       //!< Period of intra frames
    uint8_t search_radius{};//!< Search radius
    uint8_t y{};            //!< Quantization steps for Y channel
    uint8_t u{};            //!< Quantization steps for U channel
    uint8_t v{};            //!< Quantization steps for V channel
    /**
     * \brief Default constructor
     */
    LossyHybridHeader() = default;
    /**
     * \brief Constructor
     * \param header Base header to copy from
     */
    explicit LossyHybridHeader(const Header &header);
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
    static LossyHybridHeader read_header(BitStream &bs);
};

class LossyHybridEncoder final : public Encoder {
public:
    const char *src{};         ///< File path of the input video
    const char *dst{};         ///< File path of the encoded video
    LossyHybridHeader header{};///< Header object
    uint8_t golomb_m;          ///< Golomb m parameter
    uint8_t block_size;        ///< Macroblock size
    uint8_t search_radius;     ///< Search radius
    uint8_t period;            ///< Period of intra frames
    uint8_t y = 0;             ///< Quantization steps for Y channel
    uint8_t u = 0;             ///< Quantization steps for U channel
    uint8_t v = 0;             ///< Quantization steps for V channel
    Quantizer y_quant;         ///< Quantizer for Y channel
    Quantizer u_quant;         ///< Quantizer for U channel
    Quantizer v_quant;         ///< Quantizer for V channel
    LossyHybridEncoder(const char *src, const char *dst, uint8_t golomb_m, uint8_t block_size, uint8_t period, uint8_t search_radius, uint8_t y, uint8_t u, uint8_t v);
    LossyHybridEncoder(const char *src, const char *dst, uint8_t golomb_m, uint8_t block_size, uint8_t period, uint8_t y, uint8_t u, uint8_t v);
    LossyHybridEncoder(const char *src, const char *dst);
    explicit LossyHybridEncoder(const char *src);

    /**
     * \brief encodes a video from src into dst with a mix of inter and intraframe encoding
     */
    void encode() override;

    /**
     * \brief decodes a video from dst with a mix of inter and intraframe encoding
     */
    void decode() override;

    void encode_JPEG_LS(Frame &frame) const;

    void quantize_inter(Frame &frame) const;

    Frame decode_intra(Golomb &g) const;

    Frame decode_inter(Golomb &g, Frame &frame_intra) const;

    /**
     * \brief Populates encoder with data from header
     */
    void populate();

    /**
     * \brief Initializes quantizers
     */
    void initialize_quantizers();
};
