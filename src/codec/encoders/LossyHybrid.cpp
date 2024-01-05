#include "LossyHybrid.hpp"
#include "../Quantizer.hpp"

using namespace std;
using namespace cv;

LossyHybridHeader::LossyHybridHeader(const Header &header) : InterHeader() {
    this->color_space = header.color_space;
    this->chroma_subsampling = header.chroma_subsampling;
    this->height = header.height;
    this->width = header.width;
    this->golomb_m = header.golomb_m;
    this->length = header.length;
}

void LossyHybridHeader::write_header(BitStream &bs) const {
    Header::write_header(bs);
    bs.writeBits(block_size, 8);
    bs.writeBits(period, 8);
    bs.writeBits(search_radius, 8);
    bs.writeBits(y, 8);
    bs.writeBits(u, 8);
    bs.writeBits(v, 8);
}

LossyHybridHeader LossyHybridHeader::read_header(BitStream &bs) {
    LossyHybridHeader header{};
    header.color_space = static_cast<COLOR_SPACE>(bs.readBits(3));
    header.chroma_subsampling = static_cast<CHROMA_SUBSAMPLING>(bs.readBits(3));
    header.width = bs.readBits(32);
    header.height = bs.readBits(32);
    header.golomb_m = bs.readBits(8);
    header.length = bs.readBits(32);
    header.block_size = bs.readBits(8);
    header.period = bs.readBits(8);
    header.search_radius = bs.readBits(8);
    header.y = bs.readBits(8);
    header.u = bs.readBits(8);
    header.v = bs.readBits(8);
    return header;
}

LossyHybridEncoder::LossyHybridEncoder(const char *src, const char *dst, const uint8_t golomb_m, const uint8_t block_size, const uint8_t period, const uint8_t y, const uint8_t u, const uint8_t v) {
    this->src = src;
    this->dst = dst;
    this->golomb_m = golomb_m;
    this->block_size = block_size;
    this->period = period;
    this->y = y;
    this->u = u;
    this->v = v;
}

void LossyHybridEncoder::encode() {
    BitStream bs(dst, ios::out);
    Golomb g(&bs);
    const Video vid(src);
    const vector<Frame *> frames = vid.generate_frames();
    const Frame sample = *frames[0];
    header.extract_info(sample);
    header.golomb_m = golomb_m;
    header.length = frames.size();
    header.block_size = block_size;
    header.period = period;
    header.y = y;
    header.u = u;
    header.v = v;
    header.write_header(bs);
    g.set_m(golomb_m);
    int cnt = period;
    int last_intra = 0;
    Quantizer y_quant(256, y);
    Quantizer u_quant(256, u);
    Quantizer v_quant(256, v);
    for (int index = 0; index < frames.size(); index++) {
        Frame *frame = frames[index];
        if (cnt == period) {
            frame->encode_JPEG_LS();
            last_intra = index;
            cnt = 0;
            // quantize encodings
            auto encodings = frame->get_intra_encoding();
            for (int i = 0; i < encodings.size(); i++) {
                Quantizer quant;
                if (i % 3 == 0) {
                    quant = y_quant;
                } else if (i % 3 == 1) {
                    quant = u_quant;
                } else {
                    quant = v_quant;
                }
                encodings[i] = quant.quantize(encodings[i]);
            }
            frame->set_intra_encoding(encodings);
            frame->write_JPEG_LS(g);
        } else {
            Frame *frame_intra = frames[last_intra];
            frame->calculate_MV(frame_intra, block_size, header.search_radius, true);
            auto mvs = frame->get_motion_vectors();
            for (const auto &mv: mvs) {
                auto residual = mv.residual;
                residual.forEach<Vec3s>([&](Vec3s &p, const int *) -> void {
                    p[0] = y_quant.quantize(p[0]);
                    p[1] = u_quant.quantize(p[1]);
                    p[2] = v_quant.quantize(p[2]);
                });
            }
            frame->set_motion_vectors(mvs);
            frame->write(g);
            cnt++;
        }
    }
}

void LossyHybridEncoder::decode() {
    BitStream bs(src, ios::in);
    Golomb g(&bs);
    header = LossyHybridHeader::read_header(bs);
    g.set_m(header.golomb_m);
    int cnt = period;
    int last_intra = 0;
    for (int index = 0; index < header.length; index++) {
        if (cnt == period) {
            frames.push_back(decode_intra(g));
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

Frame LossyHybridEncoder::decode_intra(Golomb &g) {
    Mat mat;
    Quantizer y_quant(256, header.y);
    Quantizer u_quant(256, header.u);
    Quantizer v_quant(256, header.v);
    if (header.color_space == GRAY) {
        mat = Mat::zeros(header.height, header.width, CV_8UC1);
    } else {
        mat = Mat::zeros(header.height, header.width, CV_8UC3);
    }
    for (int r = 0; r < mat.rows; r++) {
        for (int c = 0; c < mat.cols; c++) {
            for (int channel = 0; channel < mat.channels(); channel++) {
                Quantizer quantizer;
                switch (channel) {
                    case 0:
                        quantizer = y_quant;
                        break;
                    case 1:
                        quantizer = u_quant;
                        break;
                    case 2:
                        quantizer = v_quant;
                        break;
                    default:
                        quantizer = y_quant;
                        break;
                }
                const auto diff = quantizer.get_value(g.decode());
                const uchar predicted = Frame::predict_JPEG_LS(mat, r, c, channel);
                const uchar real = diff + predicted;
                if (mat.channels() > 1) {
                    mat.at<Vec3b>(r, c)[channel] = real;
                } else {
                    mat.at<uchar>(r, c) = real;
                }
            }
        }
    }
    Image im(mat);
    im.set_color(header.color_space);
    im.set_chroma(header.chroma_subsampling);
    return Frame(im);
}