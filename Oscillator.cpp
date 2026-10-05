//
// Created by Eric El-Chabab on 2026-10-01.
//

#include "Oscillator.h"
#include <cmath>
#include <numbers>
#include <cstdint>

namespace {
    constexpr float twoPi = 2.0f * std::numbers::pi_v<float>;
}

Oscillator::Oscillator(float amplitude, float frequency, uint32_t sampleRate, Waveform waveform)
    : amplitude(amplitude), frequency(frequency), sampleRate(sampleRate), waveform(waveform) {}

float Oscillator::getNextSample() {
    const float x = phase / twoPi;
    float value = 0.0f;

    switch (waveform) {
        case Waveform::Sine:
            value = std::sin(phase);
            break;
        case Waveform::Square:
            value = (x < 0.5f) ? 1.0f : -1.0f;
            break;
        case Waveform::Saw:
            value = (2.0f * x) - 1.0f;
            break;
        case Waveform::Triangle:
            value = 1.0f - (4.0f * std::abs(x - 0.5f));
            break;
    }

    // Advance by one sample's worth of the cycle
    phase += twoPi * frequency / static_cast<float>(sampleRate);

    // Wrap around after a full cycle so phase stays small and precise
    if (phase >= twoPi) {
        phase -= twoPi;
    }

    return value * amplitude;
}