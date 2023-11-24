#include "LosslessInter.hpp"
#include "../visual/ImageProcessing.hpp"
#include "../visual/Video.hpp"

using namespace std;
using namespace cv;

void writeHeader(const InterHeader header, BitStream *bs) {
    bs->writeBits(header.color_space, 3);
    bs->writeBits(header.chroma_subsampling, 3);
    bs->writeBits(header.width, 8);
    bs->writeBits(header.height, 8);
    bs->writeBits(header.golomb_m, 8);
    bs->writeBits(header.length, 8);
    bs->writeBits(header.block_size, 8);
}

void extractInfo(const Frame &frame, InterHeader &header) {
    header.color_space = frame.getImage().getColor();
    header.chroma_subsampling = frame.getImage().getChroma();
    header.width = frame.getImage().size()[1];
    header.height = frame.getImage().size()[0];
}

InterHeader readHeader(BitStream *bs) {
    InterHeader header{};
    header.color_space = static_cast<COLOR_SPACE>(bs->readBits(3));
    header.chroma_subsampling = static_cast<CHROMA_SUBSAMPLING>(bs->readBits(3));
    header.width = bs->readBits(8);
    header.height = bs->readBits(8);
    header.golomb_m = bs->readBits(8);
    header.length = bs->readBits(8);
    header.block_size = bs->readBits(8);
    return header;
}

LosslessInterFrameEncoder::LosslessInterFrameEncoder(const char *src, const char *dst, const uint8_t golomb_m, const uint8_t block_size) : src(src), dst(dst), golomb_m(golomb_m), block_size(block_size) {}

void LosslessInterFrameEncoder::encode() {
    auto *bs = new BitStream(dst, ios::out);
    auto *golomb = new Golomb(bs);
    golomb->_set_m(golomb_m);
    const Video vid = Video(src);
    const vector<Frame *> frames = vid.generateFrames();
    const Frame sample = *frames[0];
    extractInfo(sample, header);
    header.golomb_m = golomb_m;
    header.length = frames.size();
    header.block_size = block_size;
    writeHeader(header, bs);
    Frame *last = frames[0];
    last->encode_JPEG_LS(golomb);
#pragma omp parallel for default(none) shared(frames, last)
    for (int i = 1; i < frames.size(); i++) {
        Frame *current = frames[i];
        current->calculate_MV(last, block_size, 7, false);
        last = current;
    }
    for (auto &frame: frames) {
        frame->write(golomb);
    }
    delete golomb;
}

void LosslessInterFrameEncoder::decode() {
    auto *bs = new BitStream(src, ios::in);
    auto *golomb = new Golomb(bs);
    golomb->_set_m(golomb_m);
    header = readHeader(bs);
    auto images = vector<Frame>();
    for (int i = 0; i < header.length; i++) {
        Frame img = Frame::decode_inter(golomb, &images[i - 1], header);
        images.push_back(img);
    }
}