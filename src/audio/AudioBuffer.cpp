//
// Created by Eric El-Chabab on 2026-10-01.
//

#include "AudioBuffer.h"
#include <vector>
#include <utility>


AudioBuffer::AudioBuffer(uint32_t sampleRate, uint16_t channelCount, std::vector<float> samples)
    : sampleRate(sampleRate), channelCount(channelCount), samples(std::move(samples)) {}

void AudioBuffer::addSample(float sample) {
    samples.push_back(sample);
}

uint16_t AudioBuffer::getChannelCount() const {
    return channelCount;
}

float AudioBuffer::getDuration() const {
    return static_cast<float>(samples.size()) / (static_cast<float>(sampleRate) * static_cast<float>(channelCount));
}

const std::vector<float>& AudioBuffer::getSamples() const {
    return samples;
}

uint32_t AudioBuffer::getSampleRate() const {
    return sampleRate;
}
