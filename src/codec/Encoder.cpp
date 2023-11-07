#include "Encoder.hpp"
#include "../visual/Video.hpp"

using namespace std;
using namespace cv;

void writeHeader(IntraHeader header, BitStream *bs) {
    bs->writeBits(header.color_space, 3);
    bs->writeBits(header.chroma_subsampling, 3);
    bs->writeBits(header.width, 8);
    bs->writeBits(header.height, 8);
    bs->writeBits(header.golomb_m, 8);
}

LosslessIntraFrameEncoder::LosslessIntraFrameEncoder(const char *src, const char *dst, uint8_t golomb_m) {
    this->src = const_cast<char *>(src);
    this->dst = const_cast<char *>(dst);
    this->golomb_m = golomb_m;
}

void LosslessIntraFrameEncoder::encode() {
    auto *bs = new BitStream(dst, ios::out);
    auto *golomb = new Golomb(bs);
    golomb->_set_m(golomb_m);
    Video vid = Video(src);
    vector<Frame *> frames = vid.generateFrames();
    Frame sample = *frames[0];
    header.color_space = sample.getImage()._get_color();
    header.chroma_subsampling = sample.getImage()._get_chroma();
    header.width = sample.getImage().size()[1];
    header.height = sample.getImage().size()[0];
    header.golomb_m = golomb_m;
    writeHeader(header, bs);
    for (auto &frame: frames) {
        frame->encode_JPEG_LS();
        Mat mat = *frame->getFrameMat();
        Mat channels[3];
        split(mat, channels);
        for (int i = 0; i < mat.rows; i++) {
            for (int j = 0; j < mat.cols; j++) {
                for (int k = 0; k < mat.channels(); k++) {
                    golomb->encode(channels[k].at<uchar>(i, j));
                }
            }
        }
    }
}

void LosslessIntraFrameEncoder::decode() {
    auto *bs = new BitStream(src, ios::in);
    auto *golomb = new Golomb(bs);
    golomb->_set_m(golomb_m);
}
