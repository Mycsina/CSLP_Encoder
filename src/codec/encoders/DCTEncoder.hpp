/**
    * @file LosslessHybrid.hpp
    * @brief LosslessHybrid encoder class
    */

#pragma once
#include "../Encoder.hpp"
#include "../Header.hpp"
#include "../Frame.hpp"
#include <opencv2/core/mat.hpp>

class DCTEncoder {
private:
    int (*y_qmat)[8]=new int[8][8]{
        {16,11,10,16,24,40,51,61},
        {12,12,14,19,26,58,60,55},
        {14,13,16,24,40,57,69,56},
        {14,17,22,29,51,87,80,62},
        {18,22,37,56,68,109,103,77},
        {24,35,55,64,81,104,113,92},
        {49,64,78,87,103,121,120,101},
        {72,92,95,98,112,100,103,99}
    };
    int (*uv_qmat)[8]=new int[8][8]{
            {17,18,24,47,99,99,99,99},
            {18,21,26,66,99,99,99,99},
            {24,26,56,99,99,99,99,99},
            {47,66,99,99,99,99,99,99},
            {99,99,99,99,99,99,99,99},
            {99,99,99,99,99,99,99,99},
            {99,99,99,99,99,99,99,99},
            {99,99,99,99,99,99,99,99}
    };

    //since the arrays are always the same size, hardcoding the zigzag coordinates, while hard to do, avoids having to recalculat
    int (*zigzag_order)[2]=new int[64][2]{
            {0, 0}, {0, 1}, {1, 0}, {2, 0},
            {1, 1}, {0, 2}, {0, 3}, {1, 2},
            {2, 1}, {3, 0}, {4, 0}, {3, 1},
            {2, 2}, {1, 3}, {0, 4}, {0, 5},
            {1, 4}, {2, 3}, {3, 2}, {4, 1},
            {5, 0}, {6, 0}, {5, 1}, {4, 2},
            {3, 3}, {2, 4}, {1, 5}, {0, 6},
            {0, 7}, {1, 6}, {2, 5}, {3, 4},
            {4, 3}, {5, 2}, {6, 1}, {7, 0},
            {7, 1}, {6, 2}, {5, 3}, {4, 4},
            {3, 5}, {2, 6}, {1, 7}, {2, 7},
            {3, 6}, {4, 5}, {5, 4}, {6, 3},
            {7, 2}, {7, 3}, {6, 4}, {5, 5},
            {4, 6}, {3, 7}, {4, 7}, {5, 6},
            {6, 5}, {7, 4}, {7, 5}, {6, 6},
            {5, 7}, {6, 7}, {7, 6}, {7, 7}
    };

public:
    DCTEncoder()=default;

    ~DCTEncoder();

    void encode(Video *v, BitStream *bs, int m);
    void decode(BitStream *bs);

    void encode_frame(Frame* frame, Golomb *g):
    Frame decode_frame(Golomb *g);

    void dct8x8(int (&in)[8][8], double (&out)[8][8]);
    void idct8x8(double (&in)[8][8], int (&out)[8][8]);

};
