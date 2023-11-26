#include "Video.hpp"
#include "Image.hpp"
#include "ImageProcessing.hpp"

#include <cstdio>
#include <iostream>

#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>


using namespace std;
using namespace cv;

Video::Video(const char *filename) {
    try {
        // TODO: add support for other chroma subsamplings
        load_y4m(filename, YUV420);
    } catch (runtime_error &e) {
        load(filename);
    }
}
Video::Video(const std::vector<Image> &reel) {
    im_reel = reel;
}

Video::Video(const std::vector<Frame> &frames) {
    vector<Image> reel;
    for (auto &it: frames) {
        reel.push_back(it.get_image());
    }
    im_reel = reel;
}

const vector<Image> &Video::get_reel() { return im_reel; }
void Video::set_reel(vector<Image> *reel) { im_reel = *reel; }
float Video::get_fps() const { return fps_; }
void Video::set_fps(float fps) { fps_ = fps; }

vector<Frame *> Video::generate_frames() const {
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

// ReSharper disable CppMemberFunctionMayBeConst
void Video::map(const function<void(Image &)> &func) {
    // ReSharper restore CppMemberFunctionMayBeConst
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

void Video::load_y4m(const char *filename, const CHROMA_SUBSAMPLING format) {
    int width, height, uvWidth, uvHeight;
    float fps;

    FILE *file = fopen(filename, "rb");

    if (file == nullptr) {
        throw runtime_error("Error opening file");
    }

    // get header
    Video::get_header_data(file, &width, &height, &fps);
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
        read_frame(file, width, height, uvWidth, uvHeight, format);
    }
}

void Video::read_frame(FILE *file, const int width, const int height, const int uvWidth,
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
    }
    if (string(buffer) != "FRAME\n") {
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

void Video::get_header_data(FILE *file, int *width, int *height, float *fps) {
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

void Video::play(int stop_key) const {
    if (loaded()) {
        for (auto &it: im_reel) {
            it.show(true);
            if (pollKey() == stop_key) {
                break;
            }
        }
    } else {
        throw std::runtime_error("Video hasn't been loaded");
    }
}

void Video::convert_to(const COLOR_SPACE f1, const COLOR_SPACE f2) {
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