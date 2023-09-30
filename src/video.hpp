//! @file video class declaration
/*!
 * Declares the video class, representing a digital video, composed of multiple Image objects
 */
#pragma once
#include "image.hpp"

using namespace std;
using namespace cv;

class video
{
    vector<image> im_reel;
    float fps_;
public:
    video() = default;
    ~video() = default;
    image getFrame(int pos) {
        return im_reel[pos];
    }
    void load(string filename);
    void loadY4M(const char *filename, COLOR_FORMAT format);

    //! parses the header data into the pointers (the read pointer is advanced to the byte after the header)
    //! @param *file, pointer to the file stream
    //! @param height,width,fps pointers on which to store the relevant data
    void getHeaderData(FILE* file, int* width, int* height, float* fps);
    void readFrame(FILE  *file, int width, int height, int frameSize, int uvWidth, int uvHeight, COLOR_FORMAT format);
    void play();
    void convertTo(COLOR_FORMAT f1, COLOR_FORMAT f2);

    bool loaded() const {return !im_reel.empty();};
};