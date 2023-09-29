#include "video.hpp"

#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <fstream>

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
        im_reel.push_back(*im.load(&buf));
    }
}

void video::loadY4M(string filename, COLOR_FORMAT format){
    int width,height,frameSize,uvWidth,uvHeight;
    float fps;

    ifstream file(filename);

    if(!file.is_open() || file.peek()==EOF){
        throw new std::runtime_error("File could not be opened or is empty");
    }

    //get header
    video::getHeaderData(&file, &width, &height, &fps);
    fps_=fps;

    //use yuv format to get size of frame
    switch(format){
        case YUV444:
            frameSize=width*height*3;
            uvWidth=width;
            uvHeight=height;
            break;
        case YUV422:
            frameSize=width*height*2;
            uvWidth=width/2;
            uvHeight=height;
            break;
        case YUV420:
            frameSize=width*height*3/2;
            uvWidth=width/2;
            uvHeight=height/2
            break;
        default:
            //TODO: Check if there are more fitting error types (for this and others)
            throw new std::runtime_error("Unrecognised UV format");
    }

    while(file.peek()!=EOF){ //read all frames one-by-one and add them
        readFrame(&file, width, height, frameSize, uvWidth, uvHeight);
    }
}

void video::readFrame(std::ifstream  *file, int width, int height, int frameSize, int uvWidth, int uvHeight){
}

void video::getHeaderData(std::ifstream *file, int *width, int *height, float *fps) {
    string header,discard;
    int frame_rate_num,frame_rate_den;
    getline(*file,header)
    if(sscanf(header.c_str(),"YUV4MPEG2 W%d H%d F%d:%d %s",width,height,&frame_rate_num,&frame_rate_den,&discard)!=5){
        throw new std::runtime_error("Error parsing header");
    }
    *fps=(float)frame_rate_num/(float)frame_rate_den;
}

void video::play() {
    if (loaded()) {
        for (auto & it : im_reel) {
// TODO either display image stops waiting for enter or we pass a parameter to change the behaviour
            it.display_image();
            if(cv::waitKey((int)(1/fps_)*1000==27)){ // Press ESC to stop, also ensures that the scene with the same fps (some minor variation may happen due to computation costs)
                break;
            }
        }
    }
    else
    {
        throw std::runtime_error("Video hasn't been loaded");
    }
}