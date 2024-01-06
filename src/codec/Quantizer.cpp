#include "Quantizer.hpp"
#include <cstdlib>

Quantizer::Quantizer(const int amplitude, const int steps) {
    amp = amplitude;
    step = amp / steps;
}

int Quantizer::quantize(const int value) const {
    return get_value(get_level(value));
}

int Quantizer::get_level(int value) const {
    const int sign = value < 0 ? -1 : 1;//by not quantizing in both ends, we can have around double the resolution for the same number of bits
    value = abs(value);
    return sign * ((value + step / 2) / step);
}

int Quantizer::get_value(const int level) const {
    return level * step;
}