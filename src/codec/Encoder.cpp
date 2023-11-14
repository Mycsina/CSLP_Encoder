#include "Encoder.hpp"
#include "../visual/Video.hpp"

using namespace std;
using namespace cv;

void writeHeader(IntraHeader header, BitStream *bs) {
    bs->writeBits(header.color_space, 3);
    bs->writeBits(header.chroma_subsampling, 3);
    bs->writeBits(header.width, 8);
    bs->writeBits(header.height, 8);
    bs->writeBits(header.golomb_m, 8);
    bs->writeBits(header.length, 8);
}

IntraHeader readHeader(BitStream *bs) {
    IntraHeader header{};
    header.color_space = static_cast<COLOR_SPACE>(bs->readBits(3));
    header.chroma_subsampling = static_cast<CHROMA_SUBSAMPLING>(bs->readBits(3));
    header.width = bs->readBits(8);
    header.height = bs->readBits(8);
    header.golomb_m = bs->readBits(8);
    header.length = bs->readBits(8);
    return header;
}

LosslessIntraFrameEncoder::LosslessIntraFrameEncoder(const char *src, const char *dst, uint8_t golomb_m) {
    this->src = const_cast<char *>(src);
    this->dst = const_cast<char *>(dst);
    this->golomb_m = golomb_m;
}

void LosslessIntraFrameEncoder::encode() {
    auto *bs = new BitStream(dst, ios::out);
    auto *golomb = new Golomb(bs);
    golomb->_set_m(golomb_m);
    Video vid = Video(src);
    vector<Frame *> frames = vid.generateFrames();
    Frame sample = *frames[0];
    header.color_space = sample.getImage()._get_color();
    header.chroma_subsampling = sample.getImage()._get_chroma();
    header.width = sample.getImage().size()[1];
    header.height = sample.getImage().size()[0];
    header.golomb_m = golomb_m;
    header.length = frames.size();
    writeHeader(header, bs);
    for (auto &frame: frames) {
        frame->getImage().encode_JPEG_LS(golomb);
    }
}

void LosslessIntraFrameEncoder::decode() {
    auto *bs = new BitStream(src, ios::in);
    auto *golomb = new Golomb(bs);
    auto images = vector<Image>();
    header = readHeader(bs);
    golomb->_set_m(header.golomb_m);
    for (int i = 0; i < header.length; i++) {
        Image im;
        im = Image::decode_JPEG_LS(golomb, header.color_space, header.chroma_subsampling, header.height, header.width);
        im.display_image();
        images.push_back(im);
    }
    for (auto &it: images) {
        it.display_image();
    }
}
