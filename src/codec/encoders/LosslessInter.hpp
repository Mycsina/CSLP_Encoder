#pragma once
#include "../Encoder.hpp"
#include "../Header.hpp"

class LosslessInterFrameEncoder final : public Encoder {
public:
    LosslessInterFrameEncoder(const char *src, const char *dst, uint8_t golomb_m, uint8_t block_size);
    LosslessInterFrameEncoder(const char *src, const char *dst);
    const char *src{};
    const char *dst{};
    InterHeader header{};
    uint8_t golomb_m;
    uint8_t block_size;
    void encode() override;
    void decode() override;
};