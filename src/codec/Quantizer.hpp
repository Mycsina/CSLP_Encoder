//
// Created by bruno on 11/28/23.
//

#pragma once

class Quantizer {
    int amp;
    int step;
public:
    Quantizer() = default;
    Quantizer(int amplitude, int steps);

    int get_level(int value) const;
    int get_value(int level) const;
    int quantize(int value) const;
};
