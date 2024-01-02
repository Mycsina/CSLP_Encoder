#include "../codec/Header.hpp"
#include "Frame.hpp"

Header::Header(COLOR_SPACE color_space, CHROMA_SUBSAMPLING cs, uint8_t width, uint8_t height) {
    this->color_space = color_space;
    this->chroma_subsampling = cs;
    this->width = width;
    this->height = height;
}
void Header::writeHeader(BitStream *bs) const {
    bs->writeBits(color_space, 3);
    bs->writeBits(chroma_subsampling, 3);
    bs->writeBits(width, 32);
    bs->writeBits(height, 32);
    bs->writeBits(golomb_m, 8);
    bs->writeBits(length, 32);
}
void Header::extractInfo(const Frame &frame) {
    color_space = frame.get_image().get_color();
    chroma_subsampling = frame.get_image().get_chroma();
    width = frame.get_image().size().width;
    height = frame.get_image().size().height;
}
Header Header::readHeader(BitStream *bs) {
    Header header{};
    header.color_space = static_cast<COLOR_SPACE>(bs->readBits(3));
    header.chroma_subsampling = static_cast<CHROMA_SUBSAMPLING>(bs->readBits(3));
    header.width = bs->readBits(32);
    header.height = bs->readBits(32);
    header.golomb_m = bs->readBits(8);
    header.length = bs->readBits(32);
    return header;
}

InterHeader::InterHeader(Header header) : block_size(0) {
    this->color_space = header.color_space;
    this->chroma_subsampling = header.chroma_subsampling;
    this->height = header.height;
    this->width = header.width;
    this->golomb_m = header.golomb_m;
    this->length = header.length;
}
void InterHeader::write_header(BitStream *bs) const {
    writeHeader(bs);
    bs->writeBits(block_size, 8);
}

InterHeader InterHeader::readHeader(BitStream *bs) {
    InterHeader header{};
    header.color_space = static_cast<COLOR_SPACE>(bs->readBits(3));
    header.chroma_subsampling = static_cast<CHROMA_SUBSAMPLING>(bs->readBits(3));
    header.width = bs->readBits(32);
    header.height = bs->readBits(32);
    header.golomb_m = bs->readBits(8);
    header.length = bs->readBits(32);
    header.block_size = bs->readBits(8);
    return header;
}

HybridHeader::HybridHeader(const InterHeader &header) {
    this->color_space = header.color_space;
    this->chroma_subsampling = header.chroma_subsampling;
    this->height = header.height;
    this->width = header.width;
    this->golomb_m = header.golomb_m;
    this->length = header.length;
    this->block_size = header.block_size;
}

void HybridHeader::writeHeader(BitStream *bs) const {
    write_header(bs);
    bs->writeBits(period, 8);
    bs->writeBits(search_radius, 8);
}

HybridHeader HybridHeader::readHeader(BitStream *bs) {
    HybridHeader header{};
    header.color_space = static_cast<COLOR_SPACE>(bs->readBits(3));
    header.chroma_subsampling = static_cast<CHROMA_SUBSAMPLING>(bs->readBits(3));
    header.width = bs->readBits(32);
    header.height = bs->readBits(32);
    header.golomb_m = bs->readBits(8);
    header.length = bs->readBits(32);
    header.block_size = bs->readBits(8);
    header.period = bs->readBits(8);
    header.search_radius = bs->readBits(8);
    return header;
}

LossyHybridHeader LossyHybridHeader::readHeader(BitStream *bs) {
    LossyHybridHeader header{};
    header.color_space = static_cast<COLOR_SPACE>(bs->readBits(3));
    header.chroma_subsampling = static_cast<CHROMA_SUBSAMPLING>(bs->readBits(3));
    header.width = bs->readBits(32);
    header.height = bs->readBits(32);
    header.golomb_m = bs->readBits(8);
    header.length = bs->readBits(32);
    header.block_size = bs->readBits(8);
    header.period = bs->readBits(8);
    header.num_bits = bs->readBits(8);
    header.amplitude = bs->readBits(8);
    return header;
}
