#include "BitStream.hpp"

/**
 * @brief Constructor for the BitStream class.
 * @param filePath The path to the file.
 * @param mode The open mode for the file stream.
 */
BitStream::BitStream(const std::string& filePath, std::ios_base::openmode mode) {
    file.open(filePath, mode | std::ios::binary);
    buffer = 0;
    bufferSize = 0;
}

/**
 * @brief Destructor for the BitStream class.
 */
BitStream::~BitStream() {
    if (bufferSize > 0) {
        file.put(buffer);
    }
    file.close();
}

/**
 * @brief Writes a single bit to the file.
 * @param bit The bit to be written (0 or 1).
 */
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

/**
 * @brief Reads a single bit from the file.
 * @return The read bit (0 or 1), or -1 if the end of the file is reached.
 */
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

/**
 * @brief Writes multiple bits to the file.
 * @param value The value containing the bits to be written.
 * @param n The number of bits to write.
 */
void BitStream::writeBits(int value, int n) {
    for (int i = n - 1; i >= 0; i--) {
        writeBit((value >> i) & 1);
    }
}

/**
 * @brief Reads multiple bits from the file.
 * @param n The number of bits to read.
 * @return The read value containing the bits, or -1 if the end of the file is reached.
 */
int BitStream::readBits(int n) {
    int result = 0;
    for (int i = 0; i < n; i++) {
        int bit = readBit();
        if (bit == -1) {
            return -1; // End of file
        }
        result = (result << 1) | bit;
    }
    return result;
}

/**
 * @brief Writes a string to the file.
 * @param str The string to be written.
 */
void BitStream::writeString(const std::string& str) {
    // Write the length of the string first
    writeBits(static_cast<int>(str.length()), 32);

    // Write each character of the string
    for (char c : str) {
        writeBits(static_cast<int>(c), 8);
    }
}

/**
 * @brief Reads a string from the file.
 * @return The read string.
 */
std::string BitStream::readString() {
    // Read the length of the string
    int length = readBits(32);
    if (length == -1) {
        return ""; // End of file
    }

    // Read each character of the string
    std::string str;
    for (int i = 0; i < length; i++) {
        int c = readBits(8);
        if (c == -1) {
            return ""; // End of file
        }
        str.push_back(static_cast<char>(c));
    }

    return str;
}

