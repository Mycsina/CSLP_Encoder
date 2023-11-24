#pragma once

#include "../visual/Image.hpp"

class Encoder {
public:
    virtual ~Encoder() = default;

private:
    virtual void encode() = 0;
    virtual void decode() = 0;
};