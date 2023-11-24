#pragma once
#include "../visual/Image.hpp"
#include "Frame.hpp"

class Encoder {
public:
    virtual ~Encoder() = default;
    std::vector<Frame> frames;

private:
    virtual void encode() = 0;
    virtual void decode() = 0;
};