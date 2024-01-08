#include "LossyIntra.hpp"
#include "../../Quantizer.hpp"

using namespace std;
using namespace cv;

LossyIntraHeader::LossyIntraHeader(const Header &header) : Header() {
    this->color_space = header.color_space;
    this->chroma_subsampling = header.chroma_subsampling;
    this->height = header.height;
    this->width = header.width;
    this->golomb_m = header.golomb_m;
    this->length = header.length;
}

void LossyIntraHeader::write_header(BitStream &bs) const {
    Header::write_header(bs);
    bs.writeBits(y, 8);
    bs.writeBits(u, 8);
    bs.writeBits(v, 8);
}

LossyIntraHeader LossyIntraHeader::read_header(BitStream &bs) {
    LossyIntraHeader header{};
    header.color_space = static_cast<COLOR_SPACE>(bs.readBits(3));
    header.chroma_subsampling = static_cast<CHROMA_SUBSAMPLING>(bs.readBits(3));
    header.width = bs.readBits(32);
    header.height = bs.readBits(32);
    header.golomb_m = bs.readBits(8);
    header.length = bs.readBits(32);
    header.y = bs.readBits(8);
    header.u = bs.readBits(8);
    header.v = bs.readBits(8);
    return header;
}

LossyIntraEncoder::LossyIntraEncoder(const char *src, const char *dst, const uint8_t golomb_m, const uint8_t y, const uint8_t u, const uint8_t v) {
    this->src = src;
    this->dst = dst;
    this->golomb_m = golomb_m;
    this->y = y;
    this->u = u;
    this->v = v;
    initialize_quantizers();
}

LossyIntraEncoder::LossyIntraEncoder(const char *src, const char *dst) : golomb_m(0), y_quant(), u_quant(), v_quant() {
    this->src = src;
    this->dst = dst;
}

LossyIntraEncoder::LossyIntraEncoder(const char *src) : golomb_m(0), y_quant(), u_quant(), v_quant() {
    this->src = src;
}

void LossyIntraEncoder::encode() {
    BitStream bs(dst, ios::out);
    Golomb g(&bs);
    const auto vid = Video(src);
    const vector<Frame *> frames = vid.generate_frames();
#pragma omp parallel for default(none) shared(frames)
    for (auto &frame: frames) {
        encode_JPEG_LS(*frame);
    }
    g.set_m(golomb_m);
    // Write header
    const Frame sample = *frames[0];
    header.extract_info(sample);
    header.golomb_m = golomb_m;
    header.length = frames.size();
    header.y = y;
    header.u = u;
    header.v = v;
    header.write_header(bs);
    for (auto &frame: frames) {
        frame->write_JPEG_LS(g);
    }
}

void LossyIntraEncoder::decode() {
    BitStream bs(src, ios::in);
    Golomb golomb(&bs);
    header = LossyIntraHeader::read_header(bs);
    golomb_m = header.golomb_m;
    y = header.y;
    u = header.u;
    v = header.v;
    initialize_quantizers();
    golomb.set_m(golomb_m);
    for (int i = 0; i < header.length; i++) {
        Frame img = decode_intra(golomb);
        frames.push_back(img);
    }
    if (dst != nullptr) {
        Video vid(frames);
    }
}

void LossyIntraEncoder::encode_JPEG_LS(Frame &frame) const {
    vector<int> intra_encoding;
    frame.setType(I_FRAME);
    Mat image_mat_ = *frame.get_image().get_image_mat();
    for (int r = 0; r < image_mat_.rows; r++) {
        for (int c = 0; c < image_mat_.cols; c++) {
            for (int channel = 0; channel < image_mat_.channels(); channel++) {
                const int real = image_mat_.at<Vec3b>(r, c)[channel];
                const int predicted = Frame::predict_JPEG_LS(image_mat_, r, c, channel);
                const int diff = real - predicted;
                Quantizer quant;
                if (channel == 0) {
                    quant = y_quant;
                } else if (channel == 1) {
                    quant = u_quant;
                } else {
                    quant = v_quant;
                }
                const int level = quant.get_level(diff);
                const int quantized = quant.get_value(level);
                intra_encoding.push_back(level);
                image_mat_.at<Vec3b>(r, c)[channel] = predicted + quantized;
            }
        }
    }
    frame.set_intra_encoding(intra_encoding);
}

Frame LossyIntraEncoder::decode_intra(Golomb &g) const {
    Mat mat(header.height, header.width, CV_8UC3);
    for (int r = 0; r < mat.rows; r++) {
        for (int c = 0; c < mat.cols; c++) {
            for (int channel = 0; channel < mat.channels(); channel++) {
                Quantizer quantizer;
                if (channel == 0) {
                    quantizer = y_quant;
                } else if (channel == 1) {
                    quantizer = u_quant;
                } else {
                    quantizer = v_quant;
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
    Frame frame(im);
    frame.setType(I_FRAME);
    return frame;
}

void LossyIntraEncoder::initialize_quantizers() {
    this->y_quant = Quantizer(256, y);
    this->u_quant = Quantizer(256, u);
    this->v_quant = Quantizer(256, v);
}
