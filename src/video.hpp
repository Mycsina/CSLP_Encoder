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
public:
    image getFrame(int pos) {
        return im_reel[pos];
    }
    void load(string filename);
    void play();
    bool loaded() const {return !im_reel.empty()};
};