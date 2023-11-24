#pragma once
#include "../visual/Image.hpp"

class Frame;
class Header {
public:
    COLOR_SPACE color_space;
    CHROMA_SUBSAMPLING chroma_subsampling;
    uint8_t width;
    uint8_t height;
    uint8_t golomb_m{};
    uint32_t length{};
    Header() = default;
    Header(COLOR_SPACE color_space, CHROMA_SUBSAMPLING cs, uint8_t width, uint8_t height);
    void writeHeader(BitStream *bs) const;
    void extractInfo(const Frame &frame);
    Header static readHeader(BitStream *bs);
};

class InterHeader : public Header {
public:
    uint8_t block_size;
    InterHeader() = default;
    InterHeader(Header header);
    void writeHeader(BitStream *bs) const;
    static InterHeader readHeader(BitStream *bs);
};