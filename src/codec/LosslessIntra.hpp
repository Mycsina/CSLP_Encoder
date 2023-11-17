#pragma once

#include "Encoder.hpp"


struct IntraHeader {
    COLOR_SPACE color_space;
    CHROMA_SUBSAMPLING chroma_subsampling;
    uint8_t width;
    uint8_t height;
    uint8_t golomb_m;
    uint8_t length;
};

class LosslessIntraFrameEncoder : public Encoder {
public:
    LosslessIntraFrameEncoder(const char *src, const char *dst, uint8_t golomb_m);
    char *src{};
    char *dst{};
    uint8_t golomb_m;
    IntraHeader header{};
    void encode() override;
    void decode() override;
};