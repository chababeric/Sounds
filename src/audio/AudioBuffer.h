//
// Created by Eric El-Chabab on 2026-10-01.
//

#ifndef SOUNDS_AUDIOBUFFER_H
#define SOUNDS_AUDIOBUFFER_H
#include <cstdint>
#include <vector>


class AudioBuffer {
public:
    AudioBuffer(uint32_t sampleRate, uint16_t channelCount, std::vector<float> samples = {});

    void addSample(float sample);
    uint16_t getChannelCount() const;
    float getDuration() const;
    const std::vector<float>& getSamples() const;
    uint32_t getSampleRate() const;

private:
    // How many frames are stored per second of audio (e.g. 44,100).
    // Higher = more detail, but more data. 44,100 is CD quality.
    uint32_t sampleRate;

    // Number of separate audio streams: 1 = mono, 2 = stereo (left + right).
    uint16_t channelCount;

    // The audio itself: one number per sample, each between -1.0 and 1.0.
    // In stereo, they're interleaved: left, right, left, right...
    std::vector<float> samples;
};


#endif //SOUNDS_AUDIOBUFFER_H
