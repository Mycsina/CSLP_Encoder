#include "LosslessIntra.hpp"
#include "../../visual/Video.hpp"

using namespace std;
using namespace cv;

LosslessIntraEncoder::LosslessIntraEncoder(const char *src, const char *dst) : src(src), dst(dst) {}

void LosslessIntraEncoder::encode() {
    auto *bs = new BitStream(dst, ios::out);
    auto *golomb = new Golomb(bs);
    const Video vid = Video(src);
    const vector<Frame *> frames = vid.generate_frames();
#pragma omp parallel for default(none) shared(frames)
    for (auto &frame: frames) {
        frame->encode_JPEG_LS();
    }
    // Best m
    double sum = 0;
    auto sample_f = sample_frames(frames, sample_factor);
    for (auto &frame: sample_f) {
        sum += Golomb::adjust_m(frame->get_intra_encoding(), sample_factor);
    }
    int k = static_cast<int>(sum / (frames.size() / sample_factor));
    int golomb_m = 1 << k;
    golomb->set_m(golomb_m);
    // Write header
    const Frame sample = *frames[0];
    header.extractInfo(sample);
    header.golomb_m = golomb_m;
    header.length = frames.size();
    header.writeHeader(bs);
    for (auto &frame: frames) {
        frame->write_JPEG_LS(golomb);
    }
    delete golomb;
}

void LosslessIntraEncoder::decode() {
    BitStream bs(src, ios::in);
    Golomb golomb(&bs);
    header = Header::readHeader(&bs);
    golomb.set_m(header.golomb_m);
    // TODO last frame is not decoded correctly
    for (int i = 0; i < header.length - 1; i++) {
        Frame img = Frame::decode_JPEG_LS(&golomb, header);
        frames.push_back(img);
    }
}