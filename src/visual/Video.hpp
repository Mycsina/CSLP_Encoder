//! @file Video class declaration
/*!
 * Declares the Video class, representing a digital video, composed of multiple
 * Image objects
 */
#include "../codec/Frame.hpp"
#include "Image.hpp"
#include <cstdio>
#include <functional>
#include <vector>
#pragma once

class Image;
/**
 * @brief The Video class provides methods for video reading/playing/processing
 */
class Video {
    std::vector<Image> im_reel;
    float fps_{};

public:
    Video() = default;
    ~Video() = default;
    explicit Video(const char *filename);
    const std::vector<Image> &get_reel();
    void set_reel(std::vector<Image> *reel);
    float get_fps() const;
    void set_fps(float fps);

    //! Generates vector of frames from the video
    //! @warning Must be deallocated after use
    //! @return vector of frames
    std::vector<Frame *> generateFrames();

    //! Returns frame at given position
    //! @param pos position of the frame
    //! @return Image object representing the frame
    Frame getFrame(int pos);

    //! Applies a function to every frame in the video
    //! @param func function to be applied
    void map(const std::function<void(Image &)> &func);

    //! Loads Video from file
    //! @param filename path to the file
    void load(const char *filename);

    //! Loads Video from Y4M file
    //! @param filename path to the file
    //! @param format chroma subsampling format
    void loadY4M(const char *filename, CHROMA_SUBSAMPLING format);

    //! Parses header data into given pointers (the read pointer is advanced to
    //! the byte after the header)
    //! @param file pointer to FILE object
    //! @param width pointer to the width variable
    //! @param height pointer to the height variable
    //! @param fps pointer to the fps variable
    static void getHeaderData(FILE *file, int *width, int *height, float *fps);

    //! Read a frame from an opened Y4M file
    void readFrame(FILE *file, int width, int height, int uvWidth, int uvHeight,
                   CHROMA_SUBSAMPLING format);

    //! Plays the Video
    //! @param stop_key value of the key that stops the video
    void play(int stop_key = 27);

    //! Signals whether the Video has been loaded
    //! @return true if the Video has been loaded, false otherwise
    bool loaded() const;

    //! Convert Video between color spaces
    //! @param f1 source color space
    //! @param f2 destination color space
    void convertTo(COLOR_SPACE f1, COLOR_SPACE f2);

    //! Encode video using hybrid encoding and then saves using golomb encoding
    //! @param path the path to the destination file
    //! @param m the m value for Golomb encoding
    //! @param period the number of frames between each key (intra) frame
    void encode_hybrid(const std::string& path, int m=10, int period=3);

    static Video decode_hybrid(const std::string& path);
};