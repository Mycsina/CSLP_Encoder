#include "LosslessHybrid.hpp"
#include "../../visual/ImageProcessing.hpp"
#include "../../visual/Video.hpp"

using namespace std;
using namespace cv;

HybridHeader::HybridHeader(const Header &header) : InterHeader() {
    this->color_space = header.color_space;
    this->chroma_subsampling = header.chroma_subsampling;
    this->height = header.height;
    this->width = header.width;
    this->golomb_m = header.golomb_m;
    this->length = header.length;
}

void HybridHeader::write_header(BitStream &bs) const {
    Header::write_header(bs);
    bs.writeBits(block_size, 8);
    bs.writeBits(period, 8);
    bs.writeBits(search_radius, 8);
}

HybridHeader HybridHeader::read_header(BitStream &bs) {
    HybridHeader header{};
    header.color_space = static_cast<COLOR_SPACE>(bs.readBits(3));
    header.chroma_subsampling = static_cast<CHROMA_SUBSAMPLING>(bs.readBits(3));
    header.width = bs.readBits(32);
    header.height = bs.readBits(32);
    header.golomb_m = bs.readBits(8);
    header.length = bs.readBits(32);
    header.block_size = bs.readBits(8);
    header.period = bs.readBits(8);
    header.search_radius = bs.readBits(8);
    return header;
}

LosslessHybridEncoder::LosslessHybridEncoder(const char *src, const char *dst, const uint8_t golomb_m, const uint8_t block_size, const uint8_t period) : src(src), dst(dst), golomb_m(golomb_m), block_size(block_size), period(period) {}
LosslessHybridEncoder::LosslessHybridEncoder(const char *src, const char *dst) : src(src), dst(dst), golomb_m(0), block_size(0) {}

void LosslessHybridEncoder::encode() {
    BitStream bs(dst, ios::out);
    Golomb g(&bs);
    const Video vid(src);
    const vector<Frame *> frames = vid.generate_frames();
    const Frame sample = *frames[0];
    if (golomb_m == 0) {
        // calculate golomb_m
    }
    header.extract_info(sample);
    header.golomb_m = golomb_m;
    header.length = frames.size();
    header.block_size = block_size;
    header.period = period;
    header.write_header(bs);
    g.set_m(golomb_m);
    int cnt = period;
    int last_intra = 0;
    for (int index = 0; index < frames.size(); index++) {
        Frame *frame = frames[index];
        if (cnt == period) {
            frame->encode_JPEG_LS(g);
            last_intra = index;
            cnt = 0;
        } else {
            Frame *frame_intra = frames[last_intra];
            frame->calculate_MV(frame_intra, block_size, header.search_radius, true);
            frame->write(g);
            cnt++;
        }
    }
}

void LosslessHybridEncoder::decode() {
    BitStream bs(src, ios::in);
    Golomb g(&bs);
    header = HybridHeader::read_header(bs);
    g.set_m(header.golomb_m);
    int cnt = period;
    int last_intra = 0;
    for (int index = 0; index < header.length; index++) {
        if (cnt == period) {
            frames.push_back(Frame::decode_JPEG_LS(g, static_cast<Header>(header)));
            last_intra = index;
            cnt = 0;
        } else {
            Frame frame_intra = frames[last_intra];
            header.block_size = block_size;
            frames.push_back(Frame::decode_inter(g, frame_intra, header));
            cnt++;
        }
    }
}