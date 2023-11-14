#include "Golomb.hpp"
#include <bitset>
#include <cmath>
#include <iostream>
#include <string>

using namespace std;

Golomb::Golomb(const std::string &filePath, std::ios_base::openmode mode) {
    bs = new BitStream(filePath, mode);
    filepath = filePath;
    localStream=true;
}

Golomb::Golomb(BitStream *bitstream) {
    bs = bitstream;
}

Golomb::~Golomb() { if(localStream){delete bs;}}

void Golomb::_set_m(int m_) { m = m_; }
int Golomb::_get_m() const { return m; }


int Golomb::decode() {
    if (m <= 0) {
        m = bs->readBits(8*sizeof(int));
    }
    int sign=(bs->readBit()==0)?1:-1;
    int q = readUnary();
    int r = readBinaryTrunc();
    return sign*(q * m + r);
}

void Golomb::encode(int n) {
    if (m <= 0) {
        throw std::invalid_argument("value of m unknown or invalid");
    }
    if(n<0){
        bs->writeBit(1);
    }else{
        bs->writeBit(0);
    }
    n=abs(n);
    int r = n % m;
    int q = n / m;
    writeUnary(q);
    writeBinaryTrunc(r);
}

void Golomb::encode(int n, int m_) {
    if (m <= 0) {
        m = m_;
        bs->writeBits(m_, 8*sizeof(int));
    }
    encode(n);
}

int Golomb::readUnary() {
    string buffer;
    int q = 0;
    int bit_ = -1;
    while (bit_ != 0) {
        bit_=bs->readBit();
        q++;
    }
    q--;//the last one (0) doesn't count
    return q;
}

void Golomb::writeUnary(int n) {
    for (int i = 0; i < n; i++) {
        bs->writeBit(1);
    }
    bs->writeBit(0);
}

int Golomb::readBinaryTrunc() {
    int k = floor(log2(m));
    int u = pow(k + 1, 2) - m;
    int k_bits = bs->readBits(k);
    if (k_bits < u) {
        return k_bits;
    } else {
        return ((k_bits << 1) + bs->readBit()) - u;
    }
}

void Golomb::writeBinaryTrunc(int n) {
    int k = floor(log2(m));
    int u = pow(k + 1, 2) - m;
    if (n < u)
        bs->writeBits(n, k);
    else
        bs->writeBits(n + u, k + 1);
}
