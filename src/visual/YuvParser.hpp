#pragma once

#include "Image.hpp"

#include <string>


class Video;
enum InterlaceMode {
    PROGRESSIVE,
    TOP_FIELD_FIRST,
    BOTTOM_FIELD_FIRST,
    MIXED_MODE
};

struct YuvHeader {
    int width;
    int height;
    float fps;
    InterlaceMode interlacing;
    float aspect_ratio;
    CHROMA_SUBSAMPLING color_space;
    std::string comment;
};

class YuvParser {
    YuvHeader header;
    std::string path;
    FILE *file;

public:
    explicit YuvParser(const std::string &filename);
    ~YuvParser() = default;
    static bool is_y4m(const std::string &filename);
    void parse_header();
    Video load_y4m();
    Image read_image(FILE *file, int uvWidth, int uvHeight) const;
};