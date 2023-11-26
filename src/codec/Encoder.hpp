#pragma once
#include "../visual/Image.hpp"
#include "../visual/Video.hpp"
#include "Frame.hpp"

class Encoder {
public:
    virtual ~Encoder() = default;
    std::vector<Frame> frames;

private:
    virtual void encode() = 0;
    virtual void decode() = 0;
};

inline std::vector<Frame *> sample_frames(const std::vector<Frame *> &frames, int sample_factor) {
    std::vector<Frame *> sample;
    for (int i = 0; i < frames.size() / sample_factor; i++) {
        Frame *frame = frames[rand() % frames.size()];
        sample.push_back(frame);
    }
    return sample;
}