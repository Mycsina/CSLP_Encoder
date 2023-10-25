//! @file Video class declaration
/*!
 * Declares the Video class, representing a digital video, composed of multiple
 * Image objects
 */
#pragma once

#include "Image.hpp"

using namespace std;
using namespace cv;

/**
 * @brief The Video class provides methods for video reading/playing/processing
 */
class Video {
    vector<Image> im_reel;
    float fps_;

public:
    Video() = default;

    ~Video() = default;

    vector<Image> *_get_reel() { return &im_reel; };

    void _set_reel(vector<Image> reel) { im_reel = std::move(reel); };

    float _get_fps() const { return fps_; };

    void _set_fps(float fps) { fps_ = fps; };

    //TODO this should be a frame
    //! Returns frame at given position
    //! @param pos position of the frame
    //! @return Image object representing the frame
    Image getFrame(int pos) { return Image(im_reel[pos]); }

    //! Applies a function to every frame in the video
    //! @param func function to be applied
    void map(const function<void(Image &)> &func) {
        for (auto &it: im_reel) {
            func(it);
        }
    }

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
    bool loaded() const { return !im_reel.empty(); };

    //! Convert Video between color spaces
    //! @param f1 source color space
    //! @param f2 destination color space
    void convertTo(COLOR_SPACE f1, COLOR_SPACE f2);
};