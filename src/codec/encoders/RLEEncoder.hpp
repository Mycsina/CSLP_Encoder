/**
* @file RLEEncoder.hpp
* @brief DCT encoder class
*/

#pragma once
#include "../Encoder.hpp"
#include "../Header.hpp"
#include "../Frame.hpp"
#include <opencv2/core/mat.hpp>

class RLEEncoder {
private:
    Golomb *g;
    int last_value;
    int cnt;
public:
    RLEEncoder(Golomb *g);
    ~RLEEncoder();
    void flush_buffer();
    void fetch_buffer();

    void push(int value);
    int pop();
};

