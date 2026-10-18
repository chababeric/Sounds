//
// Created by Eric El-Chabab on 2026-10-17.
//

#include <algorithm>
#include "effects/Delay.h"

namespace {
    constexpr float MAX_FEEDBACK = 0.95f;   // Must stay below 1 to avoid runaway feedback
    constexpr float MAX_MIX = 1.0f;

    constexpr float DEFAULT_DELAY_TIME = 0.3f;  // Seconds
    constexpr float DEFAULT_FEEDBACK = 0.4f;
    constexpr float DEFAULT_MIX = 0.4f;

    // Number of samples needed to hold maximumDelayTime seconds of audio.
    // Always at least 1, so the buffer is never empty (% by 0 would crash).
    size_t bufferSizeFor(uint32_t sampleRate, float maximumDelayTime) {
        const float seconds = std::max(maximumDelayTime, 0.0f);
        const auto samples = static_cast<size_t>(seconds * static_cast<float>(sampleRate));
        return std::max<size_t>(samples, 1);
    }
}

Delay::Delay(uint32_t sampleRate, float maximumDelayTime)
    : sampleRate(sampleRate),
      buffer(bufferSizeFor(sampleRate, maximumDelayTime), 0.0f) {
    setDelayTime(DEFAULT_DELAY_TIME);
    setFeedback(DEFAULT_FEEDBACK);
    setMix(DEFAULT_MIX);
}

float Delay::process(float sample) {
    // Load each setting once, so the whole sample uses consistent values
    const size_t currentDelay = delaySamples.load();
    const float currentFeedback = feedback.load();
    const float currentMix = mix.load();

    // Read the sample from currentDelay samples ago.
    // Adding buffer.size() before subtracting keeps the value from going below 0.
    const size_t readPosition = (writePosition + buffer.size() - currentDelay) % buffer.size();
    const float delayedSample = buffer[readPosition];

    // Write the current sample, plus part of the echo (feedback), for future echoes.
    // Reading happens first, so a delay of buffer.size() reads the oldest sample
    // just before it gets overwritten.
    buffer[writePosition] = sample + delayedSample * currentFeedback;

    // Move the write position forward, wrapping around at the end
    writePosition = (writePosition + 1) % buffer.size();

    // Crossfade between the original sound and the echo
    return (1.0f - currentMix) * sample + currentMix * delayedSample;
}

void Delay::setDelayTime(float delayTime) {
    // Clamp in seconds first: converting a negative float to size_t (unsigned)
    // is undefined behavior
    const float maxDelayTime = static_cast<float>(buffer.size()) / static_cast<float>(sampleRate);
    const float clampedTime = std::clamp(delayTime, 0.0f, maxDelayTime);

    // Then convert to samples, keeping at least 1:
    // a delay of 0 would read the slot being written, which holds the OLDEST sample
    const auto samples = static_cast<size_t>(clampedTime * static_cast<float>(sampleRate));
    delaySamples.store(std::clamp<size_t>(samples, 1, buffer.size()));
}

void Delay::setFeedback(float feedback) {
    this->feedback.store(std::clamp(feedback, 0.0f, MAX_FEEDBACK));
}

void Delay::setMix(float mix) {
    this->mix.store(std::clamp(mix, 0.0f, MAX_MIX));
}

float Delay::getDelayTime() const {
    return static_cast<float>(delaySamples.load()) / static_cast<float>(sampleRate);
}

float Delay::getFeedback() const {
    return feedback.load();
}

float Delay::getMix() const {
    return mix.load();
}
