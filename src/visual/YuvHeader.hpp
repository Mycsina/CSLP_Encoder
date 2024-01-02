#pragma once

#include "Image.hpp"


#include <cstdint>
#include <string>

enum CHROMA_SUBSAMPLING : std::uint8_t;
enum InterlaceMode {
    PROGRESSIVE = 'p',
    TOP_FIELD_FIRST = 't',
    BOTTOM_FIELD_FIRST = 'b',
    MIXED_MODE = 'm',
    UNKNOWN = 'u'
};

struct YuvHeader {
    int width;
    int height;
    int fps_num;
    int fps_den;
    float fps;
    InterlaceMode interlacing = UNKNOWN;
    int aspect_ratio_num = -1;
    int aspect_ratio_den = -1;
    float aspect_ratio;
    std::string raw_color_space = "-";
    CHROMA_SUBSAMPLING color_space;
    std::string comment;
};

inline void get_adjusted_dims(const YuvHeader &header, int *uvWidth, int *uvHeight) {
    const int width = header.width;
    const int height = header.height;

    switch (header.color_space) {
        case YUV444:
            *uvWidth = width;
            *uvHeight = height;
            break;
        case YUV422:
            *uvWidth = width / 2;
            *uvHeight = height;
            break;
        case YUV420:
            *uvWidth = width / 2;
            *uvHeight = height / 2;
            break;
        default:
            throw std::runtime_error("Unrecognised UV format");
    }
}