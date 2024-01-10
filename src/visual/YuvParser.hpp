/**
 * @file YuvParser.hpp
 * @brief YuvParser class header
 * @ingroup Visual
 */

#pragma once

#include <string>

#include "YuvHeader.hpp"

class Video;
class Image;

/**
 * @brief The YuvParser class provides methods for parsing YUV videos into Video objects
 */
class YuvParser {
    YuvHeader header;
    std::string path;
    FILE *file;

public:

    /**
     * \brief Constructor
     * \param filename path to the video
     */
    explicit YuvParser(const std::string &filename);
    ~YuvParser() = default;

    /**
     * \brief Check if given file is Y4M
     * \return true if the file is Y4M, false otherwise
     */
    static bool is_y4m(const std::string &filename);

    /**
     * \brief Parse YUV video header to internal YuvHeader object
     */
    void parse_header();

    /**
     * \brief Parse YUV video to Video object
     * \return Video object
     */
    Video load_y4m();

    /**
     * \brief Parse YUV frame to Image object
     * \param uvWidth width of the UV plane
     * \param uvHeight height of the UV plane
     */
    Image read_image(int uvWidth, int uvHeight) const;
};