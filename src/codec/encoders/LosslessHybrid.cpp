#include "LosslessHybrid.hpp"
#include "../../visual/ImageProcessing.hpp"
#include "../../visual/Video.hpp"

using namespace std;
using namespace cv;

LosslessHybridEncoder::LosslessHybridEncoder(const char *src, const char *dst, const uint8_t golomb_m, const uint8_t block_size, uint8_t period) : src(src), dst(dst), golomb_m(golomb_m), block_size(block_size), period(period) {}
LosslessHybridEncoder::LosslessHybridEncoder(const char *src, const char *dst) : src(src), dst(dst), golomb_m(0), block_size(0) {}


void LosslessHybridEncoder::encode() {
    BitStream bs(dst, ios::out);
    Golomb g(&bs);
    const Video vid(src);
    const vector<Frame *> frames = vid.generate_frames();
    const Frame sample = *frames[0];
    header.extractInfo(sample);
    header.golomb_m = golomb_m;
    header.length = frames.size();
    header.block_size = block_size;
    header.writeHeader(&bs);
    Frame *last = frames[0];
    g.set_m(golomb_m);
    int cnt = period;
    int last_intra = 0;
    for (int index = 0; index < frames.size(); index++) {
        Frame *frame = frames[index];
        if (cnt == period) {
            frame->encode_JPEG_LS(&g);
            last_intra = index;
            cnt = 0;
        } else {
            Frame *frame_intra = frames[last_intra];
            frame->calculate_MV(frame_intra, block_size, header.search_radius, false);
            frame->write(&g);
            cnt++;
        }
    }
}

void LosslessHybridEncoder::decode() {
    BitStream bs(src, ios::in);
    Golomb g(&bs);
    header = HybridHeader::readHeader(&bs);
    g.set_m(header.golomb_m);
    int cnt = period;
    int last_intra = 0;
    for (int index = 0; index < header.length; index++) {
        if (cnt == period) {
            frames.push_back(Frame::decode_JPEG_LS(&g, static_cast<Header>(header)));
            last_intra = index;
            cnt = 0;
        } else {
            Frame frame_intra = frames[last_intra];
            auto hd = InterHeader(static_cast<Header>(header));
            hd.block_size = block_size;
            frames.push_back(Frame::decode_inter(&g, &frame_intra, hd));
            cnt++;
        }
    }
}