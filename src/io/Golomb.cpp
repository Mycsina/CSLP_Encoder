#include "Golomb.hpp"
#include <cmath>
#include <string>

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

void Golomb::set_m(const int m_) { m = m_; }
int Golomb::get_m() const { return m; }
BitStream *Golomb::get_bs() const { return bs; }


int Golomb::decode() {
    if (m <= 0) {
        m = bs->readBits(8 * sizeof(int));
    }
    int sign = (bs->readBit() == 0) ? 1 : -1;
    int q = readUnary();
    int r = readBinaryTrunc();
    return sign * (q * m + r);
}

void Golomb::encode(int n) const {
    if (m <= 0) {
        throw std::invalid_argument("value of m unknown or invalid");
    }
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
    const int k = floor(log2(m));
    const int u = static_cast<int>(pow(k + 1, 2)) - m;
    const int k_bits = bs->readBits(k);
    if (k_bits < u) {
        return k_bits;
    }
    return ((k_bits << 1) + bs->readBit()) - u;
}

void Golomb::writeBinaryTrunc(const int n) const {
    const int k = floor(log2(m));
    const int u = static_cast<int>(pow(k + 1, 2)) - m;
    if (n < u) {
        bs->writeBits(n, k);
    } else {
        bs->writeBits(n + u, k + 1);
    }
}
