#include "video.hpp"

#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>

using namespace std;
using namespace cv;

void video::load(string filename) {
    VideoCapture cap = VideoCapture(filename);
    while (cap.isOpened()) {
        ret, frame = cap.read()
    }
}