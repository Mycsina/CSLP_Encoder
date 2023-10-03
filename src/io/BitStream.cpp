#include "BitStream.hpp"

BitStream::BitStream(const std::string& filePath, std::ios_base::openmode mode) {
    file.open(filePath, mode | std::ios::binary);
    buffer = 0;
    bufferSize = 0;
}

BitStream::~BitStream() {
    flushBuffer(); // Ensure that any remaining bits are written to the file
    file.close();
}

void BitStream::writeBit(int bit) {
    buffer <<= 1;
    buffer |= bit;
    bufferSize++;

    if (bufferSize == 8) {
        file.put(buffer);
        buffer = 0;
        bufferSize = 0;
    }
}

int BitStream::readBit() {
    if (bufferSize == 0) {
        file.read(reinterpret_cast<char*>(&buffer), 1);
        if (!file) {
            return -1; // End of file
        }
        bufferSize = 8;
    }

    int bit = (buffer >> (bufferSize - 1)) & 1;
    bufferSize--;
    return bit;
}

void BitStream::writeBits(int value, int n) {
    buffer |= value << (8 - n);
    bufferSize += n;

    flushBuffer(); // Write the buffer to the file if it's full
}

int BitStream::readBits(int n) {
    int result = 0;
    for (int i = 0; i < n; i++) {
        int bit = readBit();
        if (bit == -1) {
            return -1;
        }
        result = (result << 1) | bit;
    }
    return result;
}

void BitStream::writeString(const std::string& str) {
    writeBits(static_cast<int>(str.length()), 32);

    for (char c : str) {
        writeBits(static_cast<int>(c), 8);
    }

    flushBuffer(); // Write the buffer to the file if it's not empty
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
    if (bufferSize > 0) {
        file.put(buffer);
        buffer = 0;
        bufferSize = 0;
    }
}

