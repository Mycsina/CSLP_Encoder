#include "YuvParser.hpp"

#include "Image.hpp"
#include "Video.hpp"

#include <opencv2/core/mat.hpp>
#include <stdexcept>
#include <string>

using namespace std;
using namespace cv;

YuvParser::YuvParser(const string &filename) : header() {
    path = filename;
    file = fopen(filename.c_str(), "rb");
}

bool YuvParser::is_y4m(const string &filename) {
    FILE *file = fopen(filename.c_str(), "rb");
    if (file == nullptr) {
        throw runtime_error("Error opening file");
    }
    constexpr int bytes_to_read = 10;
    char buffer[bytes_to_read];
    if (fread(buffer, sizeof(char), bytes_to_read, file) != bytes_to_read) {
        throw runtime_error("Error reading file");
    }
    fclose(file);
    return string(buffer) == "YUV4MPEG2 ";
}

void YuvParser::parse_header() {
    constexpr int buffer_size = 200;
    char buffer[buffer_size];

    fgets(buffer, buffer_size, file);

    int frame_num, frame_den;
    int aspect_num, aspect_den;
    char interlace_mode;
    char raw_cs[10];

    const auto required = "YUV4MPEG2 W%d H%d F%d:%d I%c A%d:%d %s";
    const int required_parsed = sscanf(buffer,
                                       required,
                                       &this->header.width,
                                       &this->header.height,
                                       &frame_num,
                                       &frame_den,
                                       &interlace_mode,
                                       &aspect_num,
                                       &aspect_den,
                                       raw_cs);

    if (required_parsed < 4) {
        throw runtime_error("Error parsing header");
    }
    this->header.fps = static_cast<float>(frame_num) / static_cast<float>(frame_den);
    this->header.aspect_ratio = static_cast<float>(aspect_num) / static_cast<float>(aspect_den);
    switch (interlace_mode) {
        case 'p':
            this->header.interlacing = PROGRESSIVE;
            break;
        case 't':
            this->header.interlacing = TOP_FIELD_FIRST;
            break;
        case 'b':
            this->header.interlacing = BOTTOM_FIELD_FIRST;
            break;
        case 'm':
            this->header.interlacing = MIXED_MODE;
            break;
        default:
            throw runtime_error("Unrecognised interlace mode");
    }
    const string color_space(raw_cs);
    this->header.color_space = YUV420;
    if (color_space.find("422") != string::npos) {
        this->header.color_space = YUV422;
    } else if (color_space.find("444") != string::npos) {
        this->header.color_space = YUV444;
    }
}

Video YuvParser::load_y4m() {
    FILE *file = fopen(this->path.c_str(), "rb");

    if (file == nullptr) {
        throw runtime_error("Error opening file");
    }

    parse_header();
    int uvWidth = this->header.width;
    int uvHeight = this->header.height;

    switch (this->header.color_space) {
        case YUV444:
            break;
        case YUV422:
            uvWidth = uvWidth / 2;
            break;
        case YUV420:
            uvWidth = uvWidth / 2;
            uvHeight = uvHeight / 2;
            break;
        default:
            throw runtime_error("Unrecognised UV format");
    }

    Video video;
    video.set_fps(this->header.fps);
    int pos = 0;

    // rewind to beginning of file
    rewind(file);
    // search for char 'R' (two past start of first frame)
    while (fgetc(file) != 'R') {
    }
    // go back two bytes
    fseek(file, -2, SEEK_CUR);
    while (!feof(file)) {
        Image im = read_image(file, uvWidth, uvHeight);
        if (im.get_image_mat()->empty()) {
            break;
        }
        video.insert_image(im, pos);
        pos++;
    }
    return video;
}

Image YuvParser::read_image(FILE *file, const int uvWidth, const int uvHeight) const {
    Image im;
    char buffer[6];
    im.set_color(YUV);
    im.set_chroma(this->header.color_space);
    const int width = this->header.width;
    const int height = this->header.height;
    Mat yPlane(height, width, CV_8UC1);
    Mat uPlane(uvHeight, uvWidth, CV_8UC1);
    Mat vPlane(uvHeight, uvWidth, CV_8UC1);
    Mat frame(height, width, CV_8UC3);
    vector<Mat> channels(3);

    if (fread(buffer, sizeof(char), 6, file) != 6 && !feof(file)) {
        throw runtime_error("incomplete reading");
    }
    if (string(buffer) != "FRAME\n") {
        if (feof(file)) {
            return im;
        }
        // we're already past the FRAME, alignments are wrong
        throw runtime_error("Misaligned FRAME header");
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
    if (this->header.color_space != YUV444) {
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
    return im;
}