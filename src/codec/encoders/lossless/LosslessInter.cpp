#include "LosslessInter.hpp"
#include "../../../visual/ImageProcessing.hpp"
#include "../../../visual/Video.hpp"

using namespace std;
using namespace cv;

LosslessInterFrameEncoder::LosslessInterFrameEncoder(const char *src, const char *dst, const uint8_t golomb_m, const uint8_t block_size) : src(src), dst(dst), golomb_m(golomb_m), block_size(block_size) {}
LosslessInterFrameEncoder::LosslessInterFrameEncoder(const char *src, const char *dst) : src(src), dst(dst), golomb_m(0), block_size(0) {}


void LosslessInterFrameEncoder::encode() {
    const Video vid(src);
    BitStream bs(dst, ios::out);
    Golomb g(&bs);
    g.set_m(golomb_m);
    const vector<Frame *> frames = vid.generate_frames();
    const Frame sample = *frames[0];
    header.extract_info(sample);
    header.golomb_m = golomb_m;
    header.length = frames.size();
    header.block_size = block_size;
    header.write_header(bs);
    Frame *last = frames[0];
    last->encode_JPEG_LS(g);
#pragma omp parallel for default(none) shared(frames, last)
    for (int i = 1; i < frames.size(); i++) {
        Frame *current = frames[i];
        current->calculate_MV(*last, block_size, 7, true);
        last = current;
    }
    for (auto &frame: frames) {
        frame->write(g);
    }
}

void LosslessInterFrameEncoder::decode() {
    BitStream bs(src, ios::in);
    Golomb golomb(&bs);
    header = InterHeader::read_header(bs);
    golomb.set_m(header.golomb_m);
    frames.push_back(Frame::decode_JPEG_LS(golomb, static_cast<Header>(header))); // NOLINT(*-slicing)
    for (int i = 1; i < header.length - 1; i++) {
        Frame img = Frame::decode_inter(golomb, frames[i - 1], header);
        frames.push_back(img);
    }
}