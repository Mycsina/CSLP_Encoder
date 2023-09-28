#include "video.hpp"

#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

void video::load(string filename) {
    VideoCapture cap = VideoCapture(filename);
    while (cap.isOpened()) {
        Mat buf;
        image im;
        cap >> buf;
        if (buf.empty())
            break;
        im._set_color(BGR);
        im_reel.push_back(*im.load(&buf));

    }
}

void video::play() {
    if (loaded()) {
        for (auto & it : im_reel) {
            if (pollKey() == (int)'q') {
                break;
            }
            it.display_image(true);
        }
    }
    else
    {
        throw std::runtime_error("Video hasn't been loaded");
    }
}