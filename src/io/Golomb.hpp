/** @file Golomb.hpp
 * @brief Header file for Golomb class
 * @ingroup io
 * Declares the Golomb class, which provides methods to encode/decode int values using Golomb coding
*/

#pragma once

#include "BitStream.hpp"
#include <opencv2/core/mat.hpp>
#include <string>

/**
 * @brief The Golomb class provides methods to encode/decode int values using Golomb
 */
class Golomb {
    BitStream *bs;           ///< BitStream object
    int m = -1;              ///< m parameter of golomb code
    std::string filepath;    ///< path to file
    bool localStream = false;///< whether the BitStream object was created locally

public:
    /**
     * \brief Constructor for the Golomb class
     * \param filePath path to file
     * \param mode open mode for file stream
     */
    Golomb(const std::string &filePath, std::ios_base::openmode mode);
    /**
     * \brief Constructor for the Golomb class
     * \param bis BitStream object to use
     */
    explicit Golomb(BitStream *bis);
    /**
     * \brief Destructor for the Golomb class
     * \details Destroys the BitStream object if it was created locally
     */
    ~Golomb();
    /**
     * \brief Sets the m parameter of golomb code
     * \details Must be called before encode/decode
     * \param m_ the m parameter of golomb code
     */
    void set_m(int m_);
    /**
     * \brief Gets the m parameter of golomb code
     * \return the m parameter of golomb code
     */
    int get_m() const;
    /**
     * \brief Gets the BitStream object
     * \return BitStream object
     */
    BitStream *get_bs() const;
    //! Reads and decodes a single int from file
    //! @return the decoded int
    int decode();

    //! Encodes and writes given int
    //! @param n the int to encode
    void encode(int n) const;

    //! Encodes and writes given int
    //! @param n the int to encode
    //! @param m_ the m parameter of golomb code (only indicated once)
    void encode(int n, int m_);

    //! Reads and decodes a single int from file using unary encoding
    //! @return the decoded int
    int readUnary() const;

    //! Encodes and writes a single int to file using unary encoding
    //! @param n the int to encode
    void writeUnary(int n) const;

    //! Reads and decodes a single int from file using truncated binary
    //! @return the decoded int
    int readBinaryTrunc() const;

    //! Encodes and writes a single int to file using truncated binary
    //! @param n the int to encode
    void writeBinaryTrunc(int n) const;

    //! Finds optimal m parameter from given data points
    //! \details Uses the method described in [On the Determination of Optimal Parameterized Prefix Codes for Adaptive Entropy Coding](https://www.hpl.hp.com/techreports/2006/HPL-2006-74.pdf)
    //! @param data vector of data points
    //! @param sample_factor sample factor
    //! @return the optimal m parameter
    static int adjust_m(const std::vector<int> &data, int sample_factor);
};
