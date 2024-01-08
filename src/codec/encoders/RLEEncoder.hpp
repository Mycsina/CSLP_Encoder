/**
* @file RLEEncoder.hpp
* @brief DCT encoder class
*/

#pragma once
#include "../Encoder.hpp"

class RLEEncoder {
    Golomb *g;
    int last_value;
    int cnt;

public:
    explicit RLEEncoder(Golomb *golomb);
    ~RLEEncoder();
    void flush_buffer();
    void fetch_buffer();

    void push(int value);
    int pop();
};
