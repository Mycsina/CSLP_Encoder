#pragma once

#include "../visual/Image.hpp"

class Encoder {
    virtual void encode() = 0;
    virtual void decode() = 0;
};