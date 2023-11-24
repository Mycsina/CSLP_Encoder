#include "../src/codec/encoders/LosslessIntra.hpp"
#include "../src/visual/Video.hpp"
#include <gtest/gtest.h>
#include <iostream>

using namespace std;
using namespace cv;

class EncoderDemo : public ::testing::Test {
protected:
    const char *normal_video = "../../tests/resource/video.mp4";
    const char *small_still = "../../tests/resource/akiyo_qcif.y4m";
    const char *small_moving = "../../tests/resource/coastguard_qcif.y4m";
    void SetUp() override {
    }
};

TEST_F(EncoderDemo, losslessPredictive) {
    string file = small_still;
    string encoded = file + "_encoded";
    string decoded = file + "_decoded";
    int golomb_m = 2;
    auto *encoder = new intra_frame_encoder(file.c_str(), encoded.c_str(), golomb_m);
    encoder->encode();
    delete encoder;
    auto *decoder = new intra_frame_encoder(encoded.c_str(), decoded.c_str(), golomb_m);
    decoder->decode();
    delete decoder;
}

TEST_F(EncoderDemo, losslessIntraBestM) {
    int best_size = INFINITY;
    for (int m = 2; m < 8; m++) {
        const char *file = normal_video;
        auto *encoder = new intra_frame_encoder(file, "encoded", m);
        encoder->encode();
        delete encoder;
        ifstream testFile("encoded", ios::binary);
        auto begin = testFile.tellg();
        testFile.seekg(0, ios::end);
        const auto end = testFile.tellg();
        const auto fsize = (end - begin);
        if (fsize < best_size) {
            best_size = static_cast<int>(fsize);
            cout << "Best size: " << best_size << " with m = " << m << endl;
        }
    }
    remove("encoded");
}