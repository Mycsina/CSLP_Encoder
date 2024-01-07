#pragma once

#include <string>

#include "YuvHeader.hpp"

class Image;
class Video;
class YuvWriter {
    YuvHeader header;
    std::string path;
    FILE *file;

public:
    explicit YuvWriter(const std::string &filename);
    YuvWriter(const std::string &filename, YuvHeader header);
    void write_header() const;
    void write_image(Image &image) const;
    void write_video(Video &video);
};