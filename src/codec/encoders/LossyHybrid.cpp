#include "LossyHybrid.hpp"
#include "../Quantizer.hpp"

using namespace std;
using namespace cv;

LossyHybridEncoder::LossyHybridEncoder(const char *src, const char *dst, const uint8_t golomb_m, const uint8_t block_size, const uint8_t period, const uint8_t y, const uint8_t u, const uint8_t v) : src(src), dst(dst), golomb_m(golomb_m), block_size(block_size), period(period), y(y), u(u), v(v) {}

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
    header.y = y;
    header.u = u;
    header.v = v;
    header.writeHeader(&bs);
}


void LossyHybridEncoder::decode(){
    BitStream bs(src, ios::in);
    Golomb g(&bs);
    header = LossyHybridHeader::readHeader(&bs);
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
            auto hd = InterHeader(static_cast<Header>(header));
            hd.block_size = block_size;
            frames.push_back(Frame::decode_inter(g, frame_intra, hd));
            cnt++;
        }
    }
}

Frame LossyHybridEncoder::decode_intra(Golomb *g) {
    /*
    Mat mat;
    Quantizer quantizer();
    if (header.color_space == GRAY) {
        mat = Mat::zeros(header.height, header.width, CV_8UC1);
    } else {
        mat = Mat::zeros(header.height, header.width, CV_8UC3);
    }
    for (int r = 0; r < mat.rows; r++) {
        for (int c = 0; c < mat.cols; c++) {
            for (int channel = 0; channel < mat.channels(); channel++) {
                const auto diff = quantizer.get_value(g->decode());
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
    */
}