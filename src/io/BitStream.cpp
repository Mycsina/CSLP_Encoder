#include "BitStream.hpp"
#include <bitset>
#include <cassert>
#include <iostream>

using namespace std;

BitStream::BitStream(const string &filePath,
                     ios_base::openmode mode) {
    file.open(filePath, mode | std::ios::binary);
    if (!file.is_open()) {
        throw runtime_error("File does not exist or can't be opened");
    }
    currentByte = 0;
    buffer = vector<uint8_t>(0);
    bufferSize = 0;
}

BitStream::~BitStream() {
    flushBuffer();// Ensure that any remaining bits are written to the file
    file.close();
}

void BitStream::writeBit(int bit) {
    currentByte <<= 1;
    if (bit) {//!=0
        currentByte |= 1;
    }
    bufferSize++;
    if (bufferSize == 8) {
        buffer.push_back(currentByte);
        currentByte = 0;
        bufferSize = 0;
    }
}

int BitStream::readBit() {
    if (bufferSize == 0) {
        assert(file);
        file.read(reinterpret_cast<char *>(&currentByte), 1);
        bufferSize = 8;
    }

    const int bit = (currentByte >> (bufferSize - 1)) & 1;
    bufferSize--;
    return bit;
}

void BitStream::writeBits(int value, int n) {
    for (int i = n - 1; i >= 0; i--) {
        writeBit(value >> i & 1);//get the nth bit and write it
    }
}

int BitStream::readBits(int n) {
    int result = 0;
    for (int i = 0; i < n; i++) {
        int bit = readBit();
        result = (result << 1) | bit;
    }
    return result;
}

void BitStream::writeString(const std::string &str) {
    writeBits(static_cast<int>(str.length()), 32);

    for (char c: str) {
        writeBits(static_cast<int>(c), 8);
    }

    flushBuffer();// Write the buffer to the file if it's not empty
}

std::string BitStream::readString() {
    int length = readBits(32);
    if (length == -1) {
        return "";
    }

    std::string str;
    for (int i = 0; i < length; i++) {
        int c = readBits(8);
        if (c == -1) {
            return "";
        }
        str.push_back(static_cast<char>(c));
    }

    return str;
}

void BitStream::flushBuffer() {
    file.write(reinterpret_cast<char *>(buffer.data()), buffer.size());
    if (currentByte) {
        while (!(currentByte & 0x80)) {
            currentByte <<= 1;
        }
    }
    file.put(currentByte);
    bufferSize = 0;
    file.flush();
}

int BitStream::getPosition() {
    return file.tellg();
}
