#pragma once
#include "Encoder.hpp"

struct InterHeader {
    COLOR_SPACE color_space;
    CHROMA_SUBSAMPLING chroma_subsampling;
    uint8_t width;
    uint8_t height;
    uint8_t golomb_m;
    uint8_t length;
    uint8_t block_size;
};

class LosslessInterFrameEncoder final : public Encoder {
public:
    LosslessInterFrameEncoder(const char *src, const char *dst, uint8_t golomb_m, uint8_t block_size);
    const char *src{};
    const char *dst{};
    uint8_t golomb_m;
    uint8_t block_size;
    InterHeader header{};
    void encode() override;
    void decode() override;
};