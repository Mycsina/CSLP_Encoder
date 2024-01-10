#include "LosslessIntra.hpp"
#include "../../../visual/Video.hpp"

using namespace std;
using namespace cv;

LosslessIntraEncoder::LosslessIntraEncoder(const char *src, const char *dst, const uint8_t golomb_m)
    : src(src), dst(dst), golomb_m(golomb_m) {}
LosslessIntraEncoder::LosslessIntraEncoder(const char *src, const char *dst) : src(src), dst(dst) {}
LosslessIntraEncoder::LosslessIntraEncoder(const char *src) : src(src) {}

void LosslessIntraEncoder::encode() {
    BitStream bs(dst, ios::out);
    Golomb g(&bs);
    const auto vid = Video(src);
    const vector<Frame *> frames = vid.generate_frames();
#pragma omp parallel for default(none) shared(frames)
    for (auto &frame: frames) { frame->encode_JPEG_LS(); }
    if (golomb_m == 0) {
        // Best m
        double sum = 0;
        const auto sample_f = sample_frames(frames, sample_factor);
        for (auto &frame: sample_f) { sum += Golomb::adjust_m(frame->get_intra_encoding()); }
        const int k = static_cast<int>(sum / static_cast<double>(sample_f.size()));
        const int golomb_m = 1 << k;
        this->golomb_m = golomb_m;
    }
    g.set_m(golomb_m);
    // Write header
    const Frame sample = *frames[0];
    header.extract_info(sample);
    header.golomb_m = golomb_m;
    header.fps_num = vid.get_header().fps_num;
    header.fps_den = vid.get_header().fps_den;
    header.length = frames.size();
    header.write_header(bs);
    for (auto &frame: frames) { frame->write_JPEG_LS(g); }
}

void LosslessIntraEncoder::decode() {
    BitStream bs(src, ios::in);
    Golomb golomb(&bs);
    header = Header::read_header(bs);
    golomb.set_m(header.golomb_m);
    for (int i = 0; i < header.length; i++) {
        Frame img = Frame::decode_JPEG_LS(golomb, header);
        frames.push_back(img);
    }
    if (dst != nullptr) {
        Video vid(frames);
        vid.save_y4m(dst, header);
    }
}