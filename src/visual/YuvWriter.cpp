#include "YuvWriter.hpp"

#include "Image.hpp"
#include "Video.hpp"

using namespace std;
using namespace cv;

YuvWriter::YuvWriter(const string &filename) : header() {
    path = filename;
    file = fopen(filename.c_str(), "wb");
    if (file == nullptr) {
        throw runtime_error("Could not open file " + filename);
    }
}

void YuvWriter::write_header() const {
    fprintf(file, "YUV4MPEG2 W%d H%d F%d:%d",
            header.width,
            header.height,
            header.fps_num,
            header.fps_den);
    if (header.interlacing != UNKNOWN) {
        fprintf(file, " I%c", header.interlacing);
    }
    if (header.aspect_ratio_num != -1 && header.aspect_ratio_den != -1) {
        fprintf(file, " A%d:%d", header.aspect_ratio_num, header.aspect_ratio_den);
    }
    if (header.raw_color_space[0] != '-') {
        fprintf(file, " %s", header.raw_color_space.c_str());
    }
    fprintf(file, "\n");
}

void YuvWriter::write_image(Image &image) const {
    fprintf(file, "FRAME%c", 0x0A);
    const Mat image_mat = *image.get_image_mat();
    vector<Mat> planes;
    split(image_mat, planes);
    for (auto &plane: planes) {
        fwrite(plane.data, sizeof(uchar), plane.total(), file);
    }
}

void YuvWriter::write_video(Video &video) {
    header = video.get_header();
    write_header();
    for (auto image: video.get_reel()) {
        write_image(image);
    }
}