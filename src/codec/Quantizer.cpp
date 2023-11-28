#include "Quantizer.hpp"
#include <cstdlib>

Quantizer::Quantizer(int amplitude, int num_bits) {
    amp=amplitude;
    num_levels=2^num_bits-1;
    step=amp/num_levels;
}

int Quantizer::quantize(int value) {
    return get_value(get_level(value));
}

int Quantizer::get_level(int value) {
    int sign=value<0?-1:1; //by not quantizing in both ends, we can have around double the resolution for the same number of bits
    value=abs(value);
    return sign*((value+step/2)/step);
}

int Quantizer::get_value(int level) {
    return level * step;
}