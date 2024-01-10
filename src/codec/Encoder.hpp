/**
* @file Encoder.hpp
* @brief Encoder class
* @ingroup Codec
*/

#pragma once
#include "../visual/Image.hpp"
#include "../visual/Video.hpp"
#include "Frame.hpp"

/**
 * @brief The Encoder class provides an interface to implement video encoders.
 */
class Encoder {
public:
    virtual ~Encoder() = default;
    std::vector<Frame> frames;
    /**
     * @brief Encodes a video.
     * @details This method should be implemented by the subclass.
     * @return The encoded video.
     */
    virtual void encode() = 0;
    /**
     * @brief Decodes a video.
     * @details This method should be implemented by the subclass.
     * @return The decoded video.
     */
    virtual void decode() = 0;
};

/**
 * @brief Samples frames from a vector of frames.
 * @param frames Vector of frames to sample from.
 * @param sample_factor Factor to sample by.
 * @return Vector of sampled frames.
 */
inline std::vector<Frame *> sample_frames(const std::vector<Frame *> &frames, int sample_factor) {
    std::vector<Frame *> sample;
    for (int i = 0; i < frames.size() / sample_factor; i++) {
        Frame *frame = frames[rand() % frames.size()];
        sample.push_back(frame);
    }
    return sample;
}