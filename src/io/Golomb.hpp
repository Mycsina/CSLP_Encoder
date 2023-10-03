//! @file Golomb encoder/decoder
/*!
 * Declares the Golomb class, a class responsible for Golomb decoding/encoding
 */
#pragma once

#include <string>

using namespace std;

class Golomb{
private:
    BitStream bs;
    int m=-1;

public:
    Golomb(const string& filepath) { bs=new BitStream(filepath);}
    ~Golomb() = default;

    void _set_m(int m_) {m=m_;}
    int _get_m() {return m;}
    //! Reads and decodes a single int from file
    //! @return the decoded int
    int decode();

    //! Encodes and writes given int
    //! @param n the int to encode
    void encode(int n);

    //! Encodes and writes given int
    //! @param n the int to encode
    //! @param m_ the m parameter of golomb code (only indicated once)
    void encode(int n, int m_);

    //! Reads and decodes a single int from file using unary encoding
    //! @return the decoded int
    int readUnary();

    //! Encodes and writes a single int to file using unary encoding
    //! @param n the int to encode
    void writeUnary(int n);

    //! Reads and decodes a single int from file using truncated binary
    //! @return the decoded int
    int readBinaryTrunc();

    //! Encodes and writes a single int to file using unary encoding
    //! @param n the int to encode
    void writeBinaryTrunc(int n);

    //! converts n from an int to a binary string
    //! @param n the int to convert
    //! @param len the length of the binary string
    //! @return a binary string
    string toBinary(int n, int len);
};

