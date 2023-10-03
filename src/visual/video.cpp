#include "video.hpp"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

using namespace std;
using namespace cv;

void video::load(string filename) {
  VideoCapture cap = VideoCapture(filename);
  while (cap.isOpened()) {
    Mat buf;
    Image im;
    cap >> buf;
    if (buf.empty())
      break;
    im._set_color(BGR);
    im_reel.push_back(*im.load(buf));
  }
}

void video::loadY4M(const char *filename, CHROMA_SUBSAMPLING format) {
  int width, height, frameSize, uvWidth, uvHeight;
  float fps;

  FILE *file = fopen(filename, "rb");

  if (file == nullptr) {
    throw new std::runtime_error("Error opening file");
  }

  // get header
  video::getHeaderData(file, &width, &height, &fps);
  fps_ = fps;

  // use yuv format to get size of frame
  switch (format) {
  case YUV444:
    frameSize = width * height * 3;
    uvWidth = width;
    uvHeight = height;
    break;
  case YUV422:
    frameSize = width * height * 2;
    uvWidth = width / 2;
    uvHeight = height;
    break;
  case YUV420:
    frameSize = width * height * 3 / 2;
    uvWidth = width / 2;
    uvHeight = height / 2;
    break;
      default:
    throw new std::runtime_error("Unrecognised UV format");
  }

  while (!feof(file)) { // read all frames one-by-one and add them
    video::readFrame(file, width, height, uvWidth, uvHeight, format);
  }
}

void video::readFrame(FILE *file, int width, int height, int uvWidth,
                      int uvHeight, CHROMA_SUBSAMPLING format) {
  Image im;
  char buffer[6];
  im._set_color(YUV);
  im._set_chroma(format);
  Mat yPlane(height, width, CV_8UC1);
  Mat uPlane(uvHeight, uvWidth, CV_8UC1);
  Mat vPlane(uvHeight, uvWidth, CV_8UC1);
  Mat frame(height, width, CV_8UC3);
  vector<Mat> channels(3);

  // remove the "FRAME", if exists
  if (fread(buffer, sizeof(char), 6, file) != 6 && !feof(file)) {
    throw new std::runtime_error("incomplete reading");
  } else if (string(buffer) !=
             "FRAME\n") { // we're already past the frame (or this frame doesn't
                          // specify that a frame has started
    fseek(file, -6, SEEK_CUR);
    std::cout << buffer << " went back" << std::endl;
  }

  if (feof(file)) {
    return;
  }

  // read yPlane
  if (fread(reinterpret_cast<char *>(yPlane.data), sizeof(uint8_t),
            width * height, file) != width * height) {
    throw new runtime_error("yPlane reading not completed");
  }

  // read uPlane
  if (fread(reinterpret_cast<char *>(uPlane.data), sizeof(uint8_t),
            uvWidth * uvHeight, file) != uvWidth * uvHeight) {
    throw new std::runtime_error("uPlane reading not completed");
  }

  // read vPlane
  if (fread(reinterpret_cast<char *>(vPlane.data), sizeof(uint8_t),
            uvWidth * uvHeight, file) != uvWidth * uvHeight) {
    throw new std::runtime_error("vPlane reading not completed");
  }

  // resize u and v (if it's 4:4:4 they're already at the correct size)
  if (format != YUV444) {
    resize(uPlane, uPlane, Size(width, height));
    resize(vPlane, vPlane, Size(width, height));
  }

  // merge the three channels
  split(frame, channels);
  channels[0] = yPlane;
  channels[1] = uPlane;
  channels[2] = vPlane;
  merge(channels, frame);

  im._set_image_mat(frame);

  im_reel.push_back(im);
}

void video::getHeaderData(FILE *file, int *width, int *height, float *fps) {
  char header[90];
  char discard[200]; // should be enough for the rest
  int frame_rate_num, frame_rate_den;
  int i;

  // TODO: HEEELP, can't get proper results(even compared to the file itself)
  fgets(header, 90, file);

  if (sscanf(header, "YUV4MPEG2 W%d H%d F%d:%d %s", width, height,
             &frame_rate_num, &frame_rate_den, &discard) != 5) {
    throw new std::runtime_error("Error parsing header");
  }
  *fps = (float)frame_rate_num / (float)frame_rate_den;
}

void video::play(int stop_key) {
  if (loaded()) {
    for (auto &it : im_reel) {
      // TODO either display Image stops waiting for enter or we pass a
      // parameter to change the behaviour
      it.display_image(1);
      if (waitKey((int)(1 / fps_ * 1000 ))== stop_key) {
        // Ensures that the scene with the same fps (some minor variation may
        // happen due to computation costs)
        break;
      }
    }
  } else {
    throw std::runtime_error("Video hasn't been loaded");
  }
}

void video::convertTo(COLOR_SPACE f1, COLOR_SPACE f2) {
  vector<Image> temp;
  if (f2 == BGR && (f1 == YUV)) {
    for (int i = 0; i < im_reel.size(); i++) {
      temp.push_back(im_reel[i].convert_YUV_BGR());
    }
  }
  im_reel = temp;
}