#include "video.hpp"

#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core/mat.hpp>

using namespace std;
using namespace cv;

void video::load(string filename) {
    VideoCapture cap = VideoCapture(filename);
    while (cap.isOpened()) {
        Mat buf;
        image im;
        cap >> buf;
        im_reel.push_back(*im.load(&buf));
        if (buf.empty())
            break;
    }
}

void video::play() {
    if (loaded()) {
        for (auto & it : im_reel) {
            it.display_image();
        }
    }
    else
    {
        throw std::runtime_error("Video hasn't been loaded");
    }
}