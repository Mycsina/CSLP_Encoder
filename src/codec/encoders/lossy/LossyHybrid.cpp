#include "LossyHybrid.hpp"
#include "../../Quantizer.hpp"

using namespace std;
using namespace cv;

LossyHybridHeader::LossyHybridHeader(const Header &header) : InterHeader() {
    this->color_space = header.color_space;
    this->chroma_subsampling = header.chroma_subsampling;
    this->height = header.height;
    this->width = header.width;
    this->golomb_m = header.golomb_m;
    this->length = header.length;
    this->fps_num = header.fps_num;
    this->fps_den = header.fps_den;
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
    LossyHybridHeader header(Header::read_header(bs));
    header.block_size = bs.readBits(8);
    header.period = bs.readBits(8);
    header.search_radius = bs.readBits(8);
    header.y = bs.readBits(8);
    header.u = bs.readBits(8);
    header.v = bs.readBits(8);
    return header;
}

LossyHybridEncoder::LossyHybridEncoder(const char *src, const char *dst, const uint8_t golomb_m, const uint8_t block_size, const uint8_t period, const uint8_t search_radius, const uint8_t y, const uint8_t u, const uint8_t v) {
    this->src = src;
    this->dst = dst;
    this->golomb_m = golomb_m;
    this->block_size = block_size;
    this->period = period;
    this->search_radius = search_radius;
    this->y = y;
    this->u = u;
    this->v = v;
    initialize_quantizers();
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
    initialize_quantizers();
}
LossyHybridEncoder::LossyHybridEncoder(const char *src) : golomb_m(0), block_size(0), search_radius(0), period(0), y_quant(), u_quant(), v_quant() {
    this->src = src;
}

LossyHybridEncoder::LossyHybridEncoder(const char *src, const char *dst)
    : golomb_m(0), block_size(0), search_radius(0), period(0), y_quant(), u_quant(), v_quant() {
    this->src = src;
    this->dst = dst;
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
    header.search_radius = search_radius;
    header.y = y;
    header.u = u;
    header.v = v;
    header.write_header(bs);
    g.set_m(golomb_m);
    int cnt = period;
    int last_intra = 0;
    for (int index = 0; index < frames.size(); index++) {
        Frame *frame = frames[index];
        if (cnt == period) {
            encode_JPEG_LS(*frame);
            last_intra = index;
            cnt = 0;
        } else {
            const Frame *frame_intra = frames[last_intra];
            frame->calculate_MV(*frame_intra, block_size, header.search_radius, true);
            quantize_inter(*frame);
            cnt++;
        }
    }
    for (const auto &frame: frames) {
        if (frame->get_type() == I_FRAME) {
            frame->write_JPEG_LS(g);
        } else {
            frame->write(g);
        }
    }
}

void LossyHybridEncoder::decode() {
    BitStream bs(src, ios::in);
    Golomb g(&bs);
    header = LossyHybridHeader::read_header(bs);
    populate();
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
            frames.push_back(decode_inter(g, frame_intra));
            cnt++;
        }
        Image im = frames[index].get_image();
        im.set_color(header.color_space);
        im.set_chroma(header.chroma_subsampling);
    }
    if (dst != nullptr) {
        Video vid(frames);
        vid.save_y4m(dst, header);
    }
}

void LossyHybridEncoder::encode_JPEG_LS(Frame &frame) const {
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

void LossyHybridEncoder::quantize_inter(Frame &frame) const {
    for (auto &mv: frame.get_motion_vectors()) {
        mv.residual.forEach<Vec3s>([&](Vec3s &pixel, const int *) -> void {
            pixel[0] = y_quant.get_level(pixel[0]);
            pixel[1] = u_quant.get_level(pixel[1]);
            pixel[2] = v_quant.get_level(pixel[2]);
        });
    }
}

Frame LossyHybridEncoder::decode_intra(Golomb &g) const {
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

Frame LossyHybridEncoder::decode_inter(Golomb &g, Frame &frame_intra) const {
    vector<MotionVector> mvs;
    const int rows = header.height;
    const int cols = header.width;
    for (int block_num = 0; block_num < (rows / block_size) * (cols / block_size); block_num++) {
        MotionVector mv;
        Mat residual;
        if (header.color_space == GRAY) {
            residual = Mat::zeros(block_size, block_size, CV_16SC1);
        } else {
            residual = Mat::zeros(block_size, block_size, CV_16SC3);
        }
        mv.x = g.decode();
        mv.y = g.decode();
        for (int row = 0; row < block_size; row++) {
            for (int col = 0; col < block_size; col++) {
                for (int channel = 0; channel < residual.channels(); channel++) {
                    Quantizer quantizer;
                    if (channel == 0) {
                        quantizer = y_quant;
                    } else if (channel == 1) {
                        quantizer = u_quant;
                    } else {
                        quantizer = v_quant;
                    }
                    residual.at<Vec3s>(row, col)[channel] = static_cast<short>(quantizer.get_value(g.decode()));
                }
            }
        }
        mv.residual = residual;
        mvs.push_back(mv);
    }
    return Frame::reconstruct_frame(frame_intra, mvs, block_size);
}

void LossyHybridEncoder::populate() {
    golomb_m = header.golomb_m;
    block_size = header.block_size;
    period = header.period;
    y = header.y;
    u = header.u;
    v = header.v;
    initialize_quantizers();
}


void LossyHybridEncoder::initialize_quantizers() {
    this->y_quant = Quantizer(256, y);
    this->u_quant = Quantizer(256, u);
    this->v_quant = Quantizer(256, v);
}
