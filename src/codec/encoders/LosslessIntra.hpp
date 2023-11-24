#pragma once

#include "../Encoder.hpp"
#include "../Header.hpp"

class LosslessIntraEncoder final : public Encoder {
public:
    LosslessIntraEncoder(const char *src, const char *dst, uint8_t golomb_m);
    LosslessIntraEncoder(const char *src, const char *dst);
    const char *src{};
    const char *dst{};
    uint8_t golomb_m{};
    Header header{};
    void encode() override;
    void decode() override;
};