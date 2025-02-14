/**
 * @file Video.hpp
 * @brief Video class
 * @ingroup Visual
 */

#pragma once

#include <functional>
#include <vector>

#include "../codec/Frame.hpp"
#include "Image.hpp"
#include "YuvHeader.hpp"


class Image;
/**
 * @brief The Video class provides methods for video reading/playing/processing
 */
class Video {
    std::vector<Image> im_reel;
    float fps_{};
    YuvHeader header;

public:
    Video() = default;
    ~Video() = default;
    explicit Video(const char *filename);
    explicit Video(const std::vector<Image> &reel);
    explicit Video(const std::vector<Frame> &frames);
    const std::vector<Image> &get_reel();
    void set_reel(const std::vector<Image> *reel);
    float get_fps() const;
    void set_fps(float fps);
    YuvHeader get_header() const;
    void set_header(const YuvHeader &header);

    /**
     * \brief Returns true if the video is Y4M
     * \return true if the video is Y4M, false otherwise
     */
    bool is_y4m() const;

    /**
    * @brief Generates vector of frames from the video
    * @warning Must deallocate the frames
    * @return vector of frames
    */
    std::vector<Frame *> generate_frames() const;

    /**
     * @brief Returns frame at given position
     * @param pos position of the frame
     * @return Image object representing the frame
     */
    Frame get_frame(int pos) const;

    /**
     * @brief Inserts Image at given position
     * @param im Image to be inserted
     * @param pos position of the Image
     */
    void insert_image(const Image &im, int pos);

    /**
     * @brief Applies a function to every frame in the video
     * @param func function to be applied
     */
    void map(const std::function<void(Image &)> &func);

    /**
     * @brief Loads Video from file
     * @param filename path to the file
     */
    void load(const char *filename);

    /**
     * @brief Loads Video from Y4M file
     * @param filename path to the file
     */
    void load_y4m(const char *filename);

    /**
     * @brief Plays the Video
     * @param stop_key value of the key that stops the video
     */
    void play(int stop_key = 27) const;

    /**
     * @brief Signals whether the Video has been loaded
     * @return true if the Video has been loaded, false otherwise
     */
    bool loaded() const;

    /**
     * @brief Converts Video between color spaces
     * @param f1 source color space
     * @param f2 destination color space
     */
    void convert_to(COLOR_SPACE f1, COLOR_SPACE f2);

    /**
     * @brief Get video parameters from Encoder header
     * @param header Encoder header
     */
    void from_encoder(const Header &header);

    /**
     * @brief Write Video to file using the MJPG codec
     * @param filename path to the file
     */
    void save(const char *filename);

    /**
     * @brief Write Video to file as Y4M
     * @param filename path to the file
     * @param header Decoder header
     */
    void save_y4m(const char *filename, const Header &header);

    /**
     * @brief Compares two videos
     * @param other video to be compared with
     * @return average PSNR value
     */
    double compare(Video &other) const;
};