//! @file BitStream class declaration
/*!
 * Declares the BitStream class, which defines methods to access individual bits of a file
 */
#pragma once
#include <fstream>
#include <string>

/**
 * @brief The BitStream class provides methods for reading and writing bits to a file.
 */
class BitStream {
private:
    std::fstream file;   ///< Output file stream.
    unsigned char buffer;///< Buffer to store bits.
    int bufferSize;      ///< Current size of the buffer.

    /**
     * @brief Helper function to flush the buffer by writing its contents to the file.
     */
    void flushBuffer();

public:
    /**
     * @brief Constructor for the BitStream class.
     * @param filePath The path to the file.
     * @param mode The open mode for the file stream.
     */
    BitStream(const std::string &filePath, std::ios_base::openmode mode);

    /**
     * @brief Destructor for the BitStream class.
     * Flushes the buffer to ensure that any remaining bits are written to the file.
     */
    ~BitStream();

    /**
     * @brief Writes a single bit to the file.
     * @param bit The bit to be written (0 or 1).
     */
    void writeBit(int bit);

    /**
     * @brief Reads a single bit from the file.
     * @return The read bit (0 or 1), or -1 if the end of the file is reached.
     */
    int readBit();

    /**
     * @brief Writes multiple bits to the file.
     * @param value The value containing the bits to be written.
     * @param n The number of bits to write.
     */
    void writeBits(int value, int n);

    /**
     * @brief Reads multiple bits from the file.
     * @param n The number of bits to read.
     * @return The read value containing the bits, or -1 if the end of the file is reached.
     */
    int readBits(int n);

    /**
     * @brief Writes a string to the file.
     * @param str The string to be written.
     * Writes the length of the string followed by the binary representation of each character.
     */
    void writeString(const std::string &str);

    /**
     * @brief Reads a string from the file.
     * @return The read string, or an empty string if the end of the file is reached.
     * Reads the length of the string and then reads the binary representation of each character.
     */
    std::string readString();
};
