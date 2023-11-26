#pragma once

#include "../Encoder.hpp"
#include "../Header.hpp"

class LosslessIntraEncoder final : public Encoder {
public:
    LosslessIntraEncoder(const char *src, const char *dst);
    const char *src{};
    const char *dst{};
    Header header{};
    int sample_factor = 100;
    void encode() override;
    void decode() override;
};