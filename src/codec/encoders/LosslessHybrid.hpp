#pragma once
#include "../Encoder.hpp"
#include "../Header.hpp"

class LosslessHybridEncoder final : public Encoder {
public:
    LosslessHybridEncoder(const char *src, const char *dst, uint8_t golomb_m, uint8_t block_size, uint8_t period);
    LosslessHybridEncoder(const char *src, const char *dst);
    const char *src{};
    const char *dst{};
    InterHeader header{};
    uint8_t golomb_m;
    uint8_t block_size;
    uint8_t period{};
    void encode() override;
    void decode() override;
};