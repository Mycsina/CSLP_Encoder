#include "Golomb.hpp"
#include <cmath>
#include <string>

#define PHI 1.61803398874989484820458683436563811772030917980576286213544862270526046281890

using namespace std;

Golomb::Golomb(const std::string &filePath, const std::ios_base::openmode mode) {
    bs = new BitStream(filePath, mode);
    filepath = filePath;
    localStream = true;
}

Golomb::Golomb(BitStream *bis) {
    bs = bis;
}

Golomb::~Golomb() {
    if (localStream) { delete bs; }
}

void Golomb::set_m(const int m_) {
    m = m_;
    // REPORT: We used to be calculating k and u for every encode/decode call
    k = floor(log2(m));
    u = static_cast<int>(pow(k + 1, 2)) - m;
}
int Golomb::get_m() const { return m; }
BitStream *Golomb::get_bs() const { return bs; }


int Golomb::decode() {
    if (m <= 0) {
        m = bs->readBits(8 * sizeof(int));
        k = floor(log2(m));
        u = static_cast<int>(pow(k + 1, 2)) - m;
    }
    const int sign = (bs->readBit() == 0) ? 1 : -1;
    const int q = readUnary();
    const int r = readBinaryTrunc();
    return sign * (q * m + r);
}

void Golomb::encode(int n) const {
    assert(m > 0);
    if (n < 0) {
        bs->writeBit(1);
    } else {
        bs->writeBit(0);
    }
    n = abs(n);
    const int r = n % m;
    const int q = n / m;
    writeUnary(q);
    writeBinaryTrunc(r);
}

void Golomb::encode(const int n, const int m_) {
    k = floor(log2(m_));
    u = static_cast<int>(pow(k + 1, 2)) - m_;
    if (m <= 0) {
        m = m_;
        bs->writeBits(m_, 8 * sizeof(int));
    }
    encode(n);
}

int Golomb::readUnary() const {
    string buffer;
    int q = 0;
    int bit_ = -1;
    while (bit_ != 0) {
        bit_ = bs->readBit();
        q++;
    }
    q--;//the last one (0) doesn't count
    return q;
}

void Golomb::writeUnary(const int n) const {
    for (int i = 0; i < n; i++) {
        bs->writeBit(1);
    }
    bs->writeBit(0);
}

int Golomb::readBinaryTrunc() const {
    const int k_bits = bs->readBits(k);
    if (k_bits < u) {
        return k_bits;
    }
    const int next = bs->readBit();
    return (k_bits << 1) + next - u;
}

void Golomb::writeBinaryTrunc(const int n) const {
    if (n < u) {
        bs->writeBits(n, k);
    } else {
        bs->writeBits(n + u, k + 1);
    }
}

int Golomb::adjust_m(const std::vector<int> &data) {
    double sum = 0;
    for (int i: data) {
        sum += abs(i);
    }
    const double mean = sum / data.size();
    constexpr double golden_ratio = PHI;
    // M. Kiely, 2004
    // const int result = static_cast<int>(max(0.0, 1 + floor(log2(log(golden_ratio - 1) / log(mean / (mean + 1))))));
    // A. Said, 2006
    const int result = static_cast<int>(max(0.0, ceil(log2(mean) - 0.05 + 0.6 / mean)));
    return result;
}