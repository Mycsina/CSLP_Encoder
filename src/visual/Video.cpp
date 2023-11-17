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

vector<Frame *> Video::generateFrames() {
    vector<Frame *> frames;
    for (auto &it: im_reel) {
        frames.push_back(new Frame(it));
    }
    return frames;
}

Frame Video::getFrame(int pos) {
    return Frame(im_reel[pos]);
}

bool Video::loaded() const { return !im_reel.empty(); }

void Video::map(const function<void(Image &)> &func) {
    for (auto &it: im_reel) {
        func(it);
    }
}

void Video::load(const char *filename) {
    string ext = filename;
    VideoCapture cap = VideoCapture(ext);
    while (cap.isOpened()) {
        Mat buf;
        Image im;
        cap >> buf;
        if (buf.empty())
            break;
        im.setColor(BGR);
        im_reel.push_back(*im.load(buf));
    }
}

void Video::loadY4M(const char *filename, CHROMA_SUBSAMPLING format) {
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

void Video::readFrame(FILE *file, int width, int height, int uvWidth,
                      int uvHeight, CHROMA_SUBSAMPLING format) {
    Image im;
    char buffer[6];
    im.setColor(YUV);
    im.setChroma(format);
    Mat yPlane(height, width, CV_8UC1);
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
    if (fread(reinterpret_cast<char *>(yPlane.data), sizeof(uint8_t),
              width * height, file) != width * height) {
        throw runtime_error("yPlane reading not completed");
    }

    // read uPlane
    if (fread(reinterpret_cast<char *>(uPlane.data), sizeof(uint8_t),
              uvWidth * uvHeight, file) != uvWidth * uvHeight) {
        throw runtime_error("uPlane reading not completed");
    }

    // read vPlane
    if (fread(reinterpret_cast<char *>(vPlane.data), sizeof(uint8_t),
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

    im.setImageMat(frame);

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
    *fps = (float) frame_rate_num / (float) frame_rate_den;
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

void Video::convertTo(COLOR_SPACE f1, COLOR_SPACE f2) {
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

void Video::encode_hybrid(const std::string& path, int m, int period){
    if (loaded()) {
        auto *bs = new BitStream(path, std::ios::out);
        std::vector<int> intraBuffer,interBuffer;
        Golomb g(bs);

        Image sample_image;

        bs->writeBits(im_reel.size(),8);
        bs->writeBits(period,8);
        bs->writeBits(static_cast<int>(sample_image.getColor()), 4);
        bs->writeBits(static_cast<int>(sample_image.getChroma()), 4);
        bs->writeBits(sample_image.getImageMat()->cols, 8 * sizeof(int));
        bs->writeBits(sample_image.getImageMat()->rows, 8 * sizeof(int));
        bs->writeBits(m,8*sizeof(int));
        g._set_m(m);

        int cnt=period;
        vector<vector<int>> intra_buffer;
        vector<vector<int>> inter_buffer;

        delete bs;

    } else {
        throw std::runtime_error("Video hasn't been loaded");
    }
}
