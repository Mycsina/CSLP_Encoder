#include "LosslessHybrid.hpp"
#include "../../visual/ImageProcessing.hpp"
#include "../../visual/Video.hpp"

using namespace std;
using namespace cv;

LosslessHybridEncoder::LosslessHybridEncoder(const char *src, const char *dst, const uint8_t golomb_m, const uint8_t block_size, uint8_t period) : src(src), dst(dst), golomb_m(golomb_m), block_size(block_size), period(period) {}
LosslessHybridEncoder::LosslessHybridEncoder(const char *src, const char *dst) : src(src), dst(dst), golomb_m(0), block_size(0) {}


void LosslessHybridEncoder::encode() {
    auto *bs = new BitStream(dst, ios::out);
    auto *golomb = new Golomb(bs);
    golomb->set_m(golomb_m);
    const Video vid = Video(src);
    const vector<Frame *> frames = vid.generateFrames();
    const Frame sample = *frames[0];
    header.extractInfo(sample);
    header.golomb_m = golomb_m;
    header.length = frames.size();
    header.block_size = block_size;
    header.writeHeader(bs);
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

void LosslessHybridEncoder::decode() {
    auto *bs = new BitStream(src, ios::in);
    auto *golomb = new Golomb(bs);
    header = InterHeader::readHeader(bs);
    golomb->set_m(header.golomb_m);
    frames.push_back(Frame::decode_JPEG_LS(golomb, static_cast<Header>(header)));
    for (int i = 1; i < header.length - 1; i++) {
        Frame img = Frame::decode_inter(golomb, &frames[i - 1], header);
        frames.push_back(img);
    }
}