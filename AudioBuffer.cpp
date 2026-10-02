//
// Created by Eric El-Chabab on 2026-10-01.
//

#include "AudioBuffer.h"
#include <vector>
#include <utility>

AudioBuffer::AudioBuffer(std::vector<float> samples, float sampleRate, int channelCount)
    : samples(std::move(samples)), sampleRate(sampleRate), channelCount(channelCount) {}

void AudioBuffer::addSample(float sample) {
    samples.push_back(sample);
}

int AudioBuffer::getChannelCount() const {
    return channelCount;
}

float AudioBuffer::getDuration() const {
    return static_cast<float>(samples.size()) / sampleRate;
}

const std::vector<float>& AudioBuffer::getSamples() const {
    return samples;
}