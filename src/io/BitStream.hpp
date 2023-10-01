#ifndef BITSTREAM_HPP
#define BITSTREAM_HPP

#include <fstream>

/**
 * @brief BitStream class for reading and writing bits to a file.
 */
class BitStream {
private:
    std::ofstream file;        ///< Output file stream.
    unsigned char buffer;      ///< Buffer to store bits.
    int bufferSize;            ///< Current size of the buffer.

public:
    /**
     * @brief Constructor for the BitStream class.
     * @param filePath The path to the file.
     * @param mode The open mode for the file stream.
     */
    BitStream(const std::string& filePath, std::ios_base::openmode mode);

    /**
     * @brief Destructor for the BitStream class.
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
     */
    void writeString(const std::string& str);

    /**
     * @brief Reads a string from the file.
     * @return The read string.
     */
    std::string readString();

};

#endif // BITSTREAM_HPP
