#include "../codec/Header.hpp"
#include "../visual/Video.hpp"
#include "Frame.hpp"

Header::Header(const COLOR_SPACE color_space, const CHROMA_SUBSAMPLING cs, const uint8_t width, const uint8_t height) : golomb_m(0), length(0) {
    this->color_space = color_space;
    this->chroma_subsampling = cs;
    this->width = width;
    this->height = height;
}
void Header::write_header(BitStream &bs) const {
    bs.writeBits(color_space, 3);
    bs.writeBits(chroma_subsampling, 3);
    bs.writeBits(width, 32);
    bs.writeBits(height, 32);
    bs.writeBits(golomb_m, 8);
    bs.writeBits(length, 32);
    bs.writeBits(fps_num, 8);
    bs.writeBits(fps_den, 8);
}
void Header::extract_info(const Frame &frame) {
    color_space = frame.get_image().get_color();
    chroma_subsampling = frame.get_image().get_chroma();
    width = frame.get_image().size().width;
    height = frame.get_image().size().height;
}
Header Header::read_header(BitStream &bs) {
    Header header{};
    header.color_space = static_cast<COLOR_SPACE>(bs.readBits(3));
    header.chroma_subsampling = static_cast<CHROMA_SUBSAMPLING>(bs.readBits(3));
    header.width = bs.readBits(32);
    header.height = bs.readBits(32);
    header.golomb_m = bs.readBits(8);
    header.length = bs.readBits(32);
    header.fps_num = bs.readBits(8);
    header.fps_den = bs.readBits(8);
    return header;
}

InterHeader::InterHeader(const Header &header) : Header(), block_size(0) {
    this->color_space = header.color_space;
    this->chroma_subsampling = header.chroma_subsampling;
    this->height = header.height;
    this->width = header.width;
    this->golomb_m = header.golomb_m;
    this->length = header.length;
}
void InterHeader::write_header(BitStream &bs) const {
    Header::write_header(bs);
    bs.writeBits(block_size, 8);
}

InterHeader InterHeader::read_header(BitStream &bs) {
    InterHeader header{};
    header.color_space = static_cast<COLOR_SPACE>(bs.readBits(3));
    header.chroma_subsampling = static_cast<CHROMA_SUBSAMPLING>(bs.readBits(3));
    header.width = bs.readBits(32);
    header.height = bs.readBits(32);
    header.golomb_m = bs.readBits(8);
    header.length = bs.readBits(32);
    header.block_size = bs.readBits(8);
    return header;
}