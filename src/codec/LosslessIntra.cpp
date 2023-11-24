#include "LosslessIntra.hpp"
#include "../visual/ImageProcessing.hpp"
#include "../visual/Video.hpp"

using namespace std;
using namespace cv;

void writeHeader(const IntraHeader header, BitStream *bs) {
    bs->writeBits(header.color_space, 3);
    bs->writeBits(header.chroma_subsampling, 3);
    bs->writeBits(header.width, 8);
    bs->writeBits(header.height, 8);
    bs->writeBits(header.golomb_m, 8);
    bs->writeBits(header.length, 8);
}

void extractInfo(const Frame &frame, IntraHeader &header) {
    header.color_space = frame.getImage().getColor();
    header.chroma_subsampling = frame.getImage().getChroma();
    header.width = frame.getImage().size()[1];
    header.height = frame.getImage().size()[0];
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

LosslessIntraFrameEncoder::LosslessIntraFrameEncoder(const char *src, const char *dst, const uint8_t golomb_m) : src(src), dst(dst), golomb_m(golomb_m) {
}

void LosslessIntraFrameEncoder::encode() {
    auto *bs = new BitStream(dst, ios::out);
    auto *golomb = new Golomb(bs);
    golomb->_set_m(golomb_m);
    const Video vid = Video(src);
    const vector<Frame *> frames = vid.generateFrames();
    const Frame sample = *frames[0];
    extractInfo(sample, header);
    header.golomb_m = golomb_m;
    header.length = frames.size();
    writeHeader(header, bs);
#pragma omp parallel for default(none) shared(frames)
    for (auto &frame: frames) {
        frame->encode_JPEG_LS();
    }
    for (auto &frame: frames) {
        frame->write_JPEG_LS(golomb);
    }
    delete golomb;
}

void LosslessIntraFrameEncoder::decode() {
    auto *bs = new BitStream(src, ios::in);
    auto *golomb = new Golomb(bs);
    golomb->_set_m(golomb_m);
    header = readHeader(bs);
    auto images = vector<Frame>();
    for (int i = 0; i < header.length; i++) {
        Frame img = Frame::decode_JPEG_LS(golomb, header);
        images.push_back(img);
    }
}