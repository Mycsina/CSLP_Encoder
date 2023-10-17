//! @file Golomb encoder/decoder
/*!
 * Declares the Golomb class, a class responsible for Golomb decoding/encoding
 */
#pragma once

#include <string>

using namespace std;
/**
 * @brief The Golomb class provides methods to encode/decode int values using Golomb
 */
class Golomb{
private:
    BitStream* bs;
    int m=-1;
    string filepath;

public:
    Golomb(const std::string& filePath,std::ios_base::openmode mode) {
        bs= new BitStream(filePath,std::ios::in|std::ios::out);
        filepath=filePath;
    }
    ~Golomb() {delete bs;}

    void reset(){
        free(bs);
        bs=new BitStream(filepath,std::ios::binary);
    }

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
};

