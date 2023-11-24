#include "Video.hpp"
#include "Image.hpp"
#include "ImageProcessing.hpp"

#include <cstdio>
#include <iostream>

#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>


using namespace std;
using namespace cv;

Video::Video(const char *filename) {
    try {
        // TODO: add support for other chroma subsamplings
        Video::loadY4M(filename, YUV420);
    } catch (runtime_error &e) {
        Video::load(filename);
    }
}

const vector<Image> &Video::get_reel() { return im_reel; }
void Video::set_reel(vector<Image> *reel) { im_reel = *reel; }
float Video::get_fps() const { return fps_; }
void Video::set_fps(float fps) { fps_ = fps; }

vector<Frame *> Video::generateFrames() const {
    vector<Frame *> frames;
    for (auto &it: im_reel) {
        frames.push_back(new Frame(it));
    }
    return frames;
}

Frame Video::get_frame(const int pos) const {
    return Frame(im_reel[pos]);
}

bool Video::loaded() const { return !im_reel.empty(); }

void Video::map(const function<void(Image &)> &func) {
    for (auto &it: im_reel) {
        func(it);
    }
}

void Video::load(const char *filename) {
    const string ext = filename;
    VideoCapture cap = VideoCapture(ext);
    while (cap.isOpened()) {
        Mat buf;
        Image im;
        cap >> buf;
        if (buf.empty())
            break;
        im.set_color(BGR);
        im_reel.push_back(*im.load(buf));
    }
}

void Video::loadY4M(const char *filename, const CHROMA_SUBSAMPLING format) {
    int width, height, uvWidth, uvHeight;
    float fps;

    FILE *file = fopen(filename, "rb");

    if (file == nullptr) {
        throw runtime_error("Error opening file");
    }

    // get header
    Video::getHeaderData(file, &width, &height, &fps);
    fps_ = fps;

    // use yuv format to get size of frame
    switch (format) {
        case YUV444:
            uvWidth = width;
            uvHeight = height;
            break;
        case YUV422:
            uvWidth = width / 2;
            uvHeight = height;
            break;
        case YUV420:
            uvWidth = width / 2;
            uvHeight = height / 2;
            break;
        default:
            throw runtime_error("Unrecognised UV format");
    }

    while (!feof(file)) {// read all frames one-by-one and add them
        Video::readFrame(file, width, height, uvWidth, uvHeight, format);
    }
}

void Video::readFrame(FILE *file, const int width, const int height, const int uvWidth,
                      const int uvHeight, const CHROMA_SUBSAMPLING format) {
    Image im;
    char buffer[6];
    im.set_color(YUV);
    im.set_chroma(format);
    const Mat yPlane(height, width, CV_8UC1);
    Mat uPlane(uvHeight, uvWidth, CV_8UC1);
    Mat vPlane(uvHeight, uvWidth, CV_8UC1);
    Mat frame(height, width, CV_8UC3);
    vector<Mat> channels(3);

    // remove the "FRAME", if exists
    if (fread(buffer, sizeof(char), 6, file) != 6 && !feof(file)) {
        throw runtime_error("incomplete reading");
    } else if (string(buffer) != "FRAME\n") {
        // we're already past the frame (or this frame doesn't specify that a frame has started
        fseek(file, -6, SEEK_CUR);
        std::cout << buffer << " went back" << std::endl;
    }

    if (feof(file)) {
        return;
    }

    // read yPlane
    if (fread(yPlane.data, sizeof(uint8_t),
              width * height, file) != width * height) {
        throw runtime_error("yPlane reading not completed");
    }

    // read uPlane
    if (fread(uPlane.data, sizeof(uint8_t),
              uvWidth * uvHeight, file) != uvWidth * uvHeight) {
        throw runtime_error("uPlane reading not completed");
    }

    // read vPlane
    if (fread(vPlane.data, sizeof(uint8_t),
              uvWidth * uvHeight, file) != uvWidth * uvHeight) {
        throw runtime_error("vPlane reading not completed");
    }

    // resize u and v (if it's 4:4:4 they're already at the correct size)
    if (format != YUV444) {
        resize(uPlane, uPlane, Size(width, height));
        resize(vPlane, vPlane, Size(width, height));
    }

    // merge the three channels
    split(frame, channels);
    channels[0] = yPlane;
    channels[1] = uPlane;
    channels[2] = vPlane;
    merge(channels, frame);

    im.set_image_mat(frame);

    im_reel.push_back(im);
}

void Video::getHeaderData(FILE *file, int *width, int *height, float *fps) {
    char header[90];
    char discard[200];// should be enough for the rest
    int frame_rate_num, frame_rate_den;

    fgets(header, 90, file);

    if (sscanf(header, "YUV4MPEG2 W%d H%d F%d:%d %s", width, height,
               &frame_rate_num, &frame_rate_den, discard) != 5) {
        throw std::runtime_error("Error parsing header");
    }
    *fps = static_cast<float>(frame_rate_num) / static_cast<float>(frame_rate_den);
}

void Video::play(int stop_key) {
    if (loaded()) {
        for (auto &it: im_reel) {
            it.show(true);
            if (pollKey() == stop_key) {
                // Ensures that the scene with the same fps (some minor variation may
                // happen due to computation costs)
                break;
            }
        }
    } else {
        throw std::runtime_error("Video hasn't been loaded");
    }
}

void Video::convertTo(const COLOR_SPACE f1, const COLOR_SPACE f2) {
    function<Image(Image &)> func = [](Image &im) { return im; };
    switch (f1) {
        case BGR:
            switch (f2) {
                case BGR:
                    break;
                case YUV:
                    func = [](Image &im) { return convert_BGR_YUV444(im); };
                    break;
                case GRAY:
                    func = [](Image &im) { return convert_BGR_GRAY(im); };
                    break;
            }
            break;
        case YUV:
            switch (f2) {
                case BGR:
                    func = [](Image &im) { return convert_YUV_BGR(im); };
                    break;
                case YUV:
                    break;
                case GRAY:
                    throw runtime_error("YUV to GRAY conversion not implemented");
            }
            break;
        case GRAY:
            break;
    }
    vector<Image> temp;
    for (auto &im: im_reel) {
        temp.push_back(func(im));
    }
    im_reel = temp;
}

void Video::encode_hybrid(const std::string &path, int m, int period, int search_radius, int block_size, int threshold) const {
    if (loaded()) {
        auto *bs = new BitStream(path, std::ios::out);
        Golomb g(bs);

        Image sample_image = im_reel.front();

        //write header
        bs->writeBits(static_cast<int>(im_reel.size()), 8 * sizeof(int));
        bs->writeBits(period, 8);
        bs->writeBits(search_radius, 8);
        bs->writeBits(block_size, 8);
        bs->writeBits(static_cast<int>(fps_), 8 * sizeof(int));
        bs->writeBits(threshold, 8 * sizeof(int));
        bs->writeBits(sample_image.get_color(), 4);
        bs->writeBits(sample_image.get_chroma(), 4);
        bs->writeBits(sample_image.get_image_mat()->cols, 8 * sizeof(int));
        bs->writeBits(sample_image.get_image_mat()->rows, 8 * sizeof(int));
        bs->writeBits(m, 8 * sizeof(int));
        g.set_m(m);
        //encode in bulk into the buffers
        int cnt = period;
        int last_intra = 0;
        for (int index = 0; index < im_reel.size(); index++) {
            Frame frame(im_reel[index]);
            if (cnt == period) {
                frame.encode_JPEG_LS(&g);
                last_intra = index;
                cnt = 0;
            } else {
                Frame frame_intra(im_reel[last_intra]);
                frame.calculate_MV(&frame_intra, block_size, search_radius, false);
                auto mv = frame.getMotionVectors().at(1);
                frame.write(&g);
                cnt++;
            }
        }
        delete bs;
    } else {
        throw std::runtime_error("Video hasn't been loaded");
    }
}

Video Video::decode_hybrid(const std::string &path) {
    auto *bs = new BitStream(path, std::ios::in);
    auto *im_reel = new vector<Image>();
    Golomb g(bs);
    Video v;

    //read header
    int size = bs->readBits(8 * sizeof(int));
    int period = bs->readBits(8);
    int search_radius = bs->readBits(8);
    int block_size = bs->readBits(8);
    int fps_ = bs->readBits(8 * sizeof(int));
    int threshold = bs->readBits(8 * sizeof(int));
    auto c_space = static_cast<COLOR_SPACE>(bs->readBits(4));
    auto cs_ratio = static_cast<CHROMA_SUBSAMPLING>(bs->readBits(4));
    int cols = bs->readBits(8 * sizeof(int));
    int rows = bs->readBits(8 * sizeof(int));
    int m = bs->readBits(8 * sizeof(int));
    Header header = Header(c_space, cs_ratio, cols, rows);
    g.set_m(m);

    int cnt = period;
    int last_intra = 0;
    for (int index = 0; index < size; index++) {
        if (cnt == period) {
            im_reel->push_back(Frame::decode_JPEG_LS(&g, header).getImage());
            last_intra = index;
            cnt = 0;
        } else {
            Frame frame_intra((*im_reel)[last_intra]);
            auto hd = InterHeader(header);
            hd.block_size = block_size;
            im_reel->push_back(Frame::decode_inter(&g, &frame_intra, hd).getImage());
            cnt++;
        }
    }
    v.set_fps(fps_);
    v.set_reel(im_reel);
    return v;
}