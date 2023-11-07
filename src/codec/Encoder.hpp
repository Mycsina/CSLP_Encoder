#pragma once

#include "../visual/Image.hpp"

class Encoder {
    virtual void encode() = 0;
    virtual void decode() = 0;
};

struct IntraHeader {
    COLOR_SPACE color_space;
    CHROMA_SUBSAMPLING chroma_subsampling;
    uint8_t width;
    uint8_t height;
    uint8_t golomb_m;
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