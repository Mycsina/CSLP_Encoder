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
    video() = default;
    ~video() = default;
    vector<image> _get_reel() {return im_reel;};
    void _set_reel(vector<image> reel) {im_reel = std::move(reel);};
    image getFrame(int pos) {
        return im_reel[pos];
    }
    void map(function<void(image&)> func) {
        for (auto & it : im_reel) {
            func(it);
        }
    }
    void load(string filename);
    void play();
    bool loaded() const {return !im_reel.empty();};
};