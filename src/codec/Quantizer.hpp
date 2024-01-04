//
// Created by bruno on 11/28/23.
//

#pragma once

class Quantizer {
private:
    int amp;
    int num_levels;
    int step;
public:
    Quantizer(int amplitude, int num_bits);

    int get_level(int value);
    int get_value(int level);
    int quantize(int value);
};
