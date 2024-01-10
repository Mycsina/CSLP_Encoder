/**
* @file YuvWriter.hpp
* @brief YuvWriter class
* @ingroup Visual
*/
#pragma once

#include <string>

#include "YuvHeader.hpp"

class Image;
class Video;
/**
 * \brief The YuvWriter class provides methods for writing YUV videos
 */
class YuvWriter {
    YuvHeader header;
    std::string path;
    FILE *file;

public:
    ~YuvWriter() = default;
    /**
     * \brief Constructor
     * \param filename path to where the video will be written
     */
    explicit YuvWriter(const std::string &filename);
    /**
     * \brief Constructor
     * \param filename path to where the video will be written
     * \param header YuvHeader object
     */
    YuvWriter(const std::string &filename, YuvHeader header);
    /**
     * \brief Writes YuvHeader to filepath specified in constructor
     */
    void write_header() const;
    /**
     * \brief Writes Image to filepath specified in constructor
     * \param image Image to be written
     */
    void write_image(Image &image) const;
    /**
     * \brief Writes Video to filepath specified in constructor
     * \param video Video to be written
     */
    void write_video(Video &video);
};