//
// Created by Eric El-Chabab on 2026-10-01.
//

#ifndef SOUNDS_AUDIOBUFFER_H
#define SOUNDS_AUDIOBUFFER_H
#include <vector>


class AudioBuffer {
public:
    AudioBuffer(std::vector<float> samples, float sampleRate, int channelCount);

    void addSample(float sample);
    int getChannelCount() const;
    float getDuration() const;
    const std::vector<float>& getSamples() const;

private:
    std::vector<float> samples;
    float sampleRate;
    int channelCount;
};


#endif //SOUNDS_AUDIOBUFFER_H
