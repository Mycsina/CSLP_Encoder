#pragma once

#include <string>

#include "YuvHeader.hpp"

class Video;
class Image;

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