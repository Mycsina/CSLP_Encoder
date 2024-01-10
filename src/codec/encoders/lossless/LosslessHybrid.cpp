#include "LosslessHybrid.hpp"
#include "../../../visual/ImageProcessing.hpp"
#include "../../../visual/Video.hpp"

using namespace std;
using namespace cv;

HybridHeader::HybridHeader(const Header &header) : InterHeader() {
    this->color_space = header.color_space;
    this->chroma_subsampling = header.chroma_subsampling;
    this->height = header.height;
    this->width = header.width;
    this->golomb_m = header.golomb_m;
    this->length = header.length;
    this->fps_num = header.fps_num;
    this->fps_den = header.fps_den;
}

void HybridHeader::write_header(BitStream &bs) const {
    Header::write_header(bs);
    bs.writeBits(block_size, 8);
    bs.writeBits(period, 8);
    bs.writeBits(search_radius, 8);
}

HybridHeader HybridHeader::read_header(BitStream &bs) {
    HybridHeader header(Header::read_header(bs));
    header.block_size = bs.readBits(8);
    header.period = bs.readBits(8);
    header.search_radius = bs.readBits(8);
    return header;
}

LosslessHybridEncoder::LosslessHybridEncoder(const char *src, const char *dst, const uint8_t golomb_m,
                                             const uint8_t block_size, const uint8_t period)
    : src(src), dst(dst), golomb_m(golomb_m), block_size(block_size), period(period) {}
LosslessHybridEncoder::LosslessHybridEncoder(const char *src, const char *dst)
    : src(src), dst(dst), golomb_m(0), block_size(0) {}
LosslessHybridEncoder::LosslessHybridEncoder(const char *src) : src(src), golomb_m(0), block_size(0) {}

void LosslessHybridEncoder::encode() {
    BitStream bs(dst, ios::out);
    Golomb g(&bs);
    const Video vid(src);
    const vector<Frame *> frames = vid.generate_frames();
    const Frame sample = *frames[0];
    int cnt = period;
    int last_intra = 0;
    vector<Frame *> intra_frames;
    vector<Frame *> inter_frames;
    for (int index = 0; index < frames.size(); index++) {
        Frame *frame = frames[index];
        if (cnt == period) {
            frame->encode_JPEG_LS();
            intra_frames.push_back(frame);
            last_intra = index;
            cnt = 0;
        } else {
            const Frame *frame_intra = frames[last_intra];
            inter_frames.push_back(frame);
            frame->calculate_MV(*frame_intra, block_size, header.search_radius, true);
            cnt++;
        }
    }
    if (golomb_m == 0) {
        // Best m
        double sum = 0;
        const auto intra = sample_frames(intra_frames, sample_factor);
        const auto inter = sample_frames(inter_frames, sample_factor);
        for (auto &frame: intra) { sum += Golomb::adjust_m(frame->get_intra_encoding()); }
        for (auto &frame: inter) {
            vector<int> inter_encoding;
            for (auto &mv: frame->get_motion_vectors()) {
                inter_encoding.push_back(mv.x);
                inter_encoding.push_back(mv.y);
                for (int row = 0; row < block_size; row++) {
                    for (int col = 0; col < block_size; col++) {
                        auto residual = mv.residual.at<Vec3s>(row, col);
                        inter_encoding.push_back(residual[0]);
                        inter_encoding.push_back(residual[1]);
                        inter_encoding.push_back(residual[2]);
                    }
                }
            }
            sum += Golomb::adjust_m(inter_encoding);
        }
        auto size = intra.size() + inter.size();
        const int k = static_cast<int>(sum / static_cast<double>(size));
        const int golomb_m = 1 << k;
        this->golomb_m = golomb_m;
    }
    g.set_m(golomb_m);
    header.extract_info(sample);
    header.golomb_m = golomb_m;
    header.length = frames.size();
    header.block_size = block_size;
    header.period = period;
    header.write_header(bs);
    // REPORT: moving writes to separate loop (-10s)
    for (auto &frame: frames) {
        if (frame->get_type() == P_FRAME) {
            frame->write(g);
        } else {
            frame->write_JPEG_LS(g);
        }
    }
}

void LosslessHybridEncoder::decode() {
    BitStream bs(src, ios::in);
    Golomb g(&bs);
    header = HybridHeader::read_header(bs);
    block_size = header.block_size;
    period = header.period;
    golomb_m = header.golomb_m;
    g.set_m(header.golomb_m);
    int cnt = period;
    int last_intra = 0;
    for (int index = 0; index < header.length; index++) {
        if (cnt == period) {
            frames.push_back(Frame::decode_JPEG_LS(g, static_cast<Header>(header))); // NOLINT(*-slicing)
            last_intra = index;
            cnt = 0;
        } else {
            Frame frame_intra = frames[last_intra];
            header.block_size = block_size;
            frames.push_back(Frame::decode_inter(g, frame_intra, header));
            cnt++;
        }
    }
    if (dst != nullptr) {
        Video vid(frames);
        vid.save_y4m(dst, header);
    }
}