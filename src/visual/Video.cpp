#include "Video.hpp"
#include "Image.hpp"
#include "ImageProcessing.hpp"
#include "YuvParser.hpp"
#include "YuvWriter.hpp"

#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>


using namespace std;
using namespace cv;

Video::Video(const char *filename) {
    if (YuvParser::is_y4m(filename)) {
        load_y4m(filename);
    } else {
        load(filename);
    }
}
Video::Video(const std::vector<Image> &reel) : header() { im_reel = reel; }

Video::Video(const std::vector<Frame> &frames) : header() {
    vector<Image> reel;
    for (auto &it: frames) { reel.push_back(it.get_image()); }
    im_reel = reel;
}

const vector<Image> &Video::get_reel() { return im_reel; }
void Video::set_reel(const vector<Image> *reel) { im_reel = *reel; }
float Video::get_fps() const { return fps_; }
void Video::set_fps(const float fps) { fps_ = fps; }
YuvHeader Video::get_header() const { return header; }
void Video::set_header(const YuvHeader &header) { Video::header = header; }
bool Video::is_y4m() const { return im_reel[0].get_color() == YUV; }

vector<Frame *> Video::generate_frames() const {
    vector<Frame *> frames;
    for (auto &it: im_reel) { frames.push_back(new Frame(it)); }
    return frames;
}

Frame Video::get_frame(const int pos) const { return Frame(im_reel[pos]); }

void Video::insert_image(const Image &im, const int pos) { im_reel.insert(im_reel.begin() + pos, im); }

bool Video::loaded() const { return !im_reel.empty(); }

// ReSharper disable CppMemberFunctionMayBeConst
void Video::map(const function<void(Image &)> &func) {
    // ReSharper restore CppMemberFunctionMayBeConst
    for (auto &it: im_reel) { func(it); }
}

void Video::load(const char *filename) {
    const string ext = filename;
    auto cap = VideoCapture(ext);
    while (cap.isOpened()) {
        Mat buf;
        Image im;
        cap >> buf;
        if (buf.empty()) break;
        im.set_color(BGR);
        im_reel.push_back(*im.load(buf));
    }
}

void Video::load_y4m(const char *filename) {
    auto parser = YuvParser(filename);
    Video video = parser.load_y4m();
    im_reel = video.get_reel();
    fps_ = video.get_fps();
    header = video.get_header();
}

void Video::play(const int stop_key) const {
    if (loaded()) {
        for (auto &it: im_reel) {
            it.show(true);
            if (pollKey() == stop_key) { break; }
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
    for (auto &im: im_reel) { temp.push_back(func(im)); }
    im_reel = temp;
}

void Video::from_encoder(const Header &header) {
    this->header.width = header.width;
    this->header.height = header.height;
    this->header.fps_num = header.fps_num;
    this->header.fps_den = header.fps_den;
    this->header.color_space = header.chroma_subsampling;
}

void Video::save(const char *filename) {
    const string ext = filename;
    const auto fourcc = VideoWriter::fourcc('M', 'J', 'P', 'G');
    auto writer = VideoWriter(ext, fourcc, fps_, Size(header.width, header.height));
    for (auto &it: im_reel) { writer.write(*it.get_image_mat()); }
}

void Video::save_y4m(const char *filename, const Header &header) {
    const string ext = filename;
    from_encoder(header);
    YuvWriter writer(filename, this->header);
    writer.write_video(*this);
}

double Video::compare(Video &other) const {
    if (loaded() && other.loaded()) {
        if (im_reel.size() != other.get_reel().size()) { return INFINITY; }
        auto mse = [](const Image &im1, const Image &im2) {
            double sum = 0;
            for (int i = 0; i < im1.size().height; i++) {
                for (int j = 0; j < im1.size().width; j++) {
                    auto p1 = im1.getPixel(i, j);
                    auto p2 = im2.getPixel(i, j);
                    sum += pow(p1[0] - p2[0], 2) + pow(p1[1] - p2[1], 2) + pow(p1[2] - p2[2], 2);
                }
            }
            return sum / (im1.size().height * im1.size().width);
        };
        auto psnr = [](const double mse) -> double {
            if (mse == 0) return INFINITY;
            return 10 * log10(pow(255, 2) / mse);
        };
        double sum = 0;
        for (int i = 0; i < im_reel.size(); i++) { sum += psnr(mse(im_reel[i], other.get_reel()[i])); }
        return sum / im_reel.size();
    }
    throw std::runtime_error("Video hasn't been loaded");
}
