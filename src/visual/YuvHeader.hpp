#pragma once

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