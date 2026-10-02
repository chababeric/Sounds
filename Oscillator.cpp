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

Oscillator::Oscillator(float amplitude, float frequency, uint32_t sampleRate)
    : amplitude(amplitude), frequency(frequency), sampleRate(sampleRate) {}

float Oscillator::getNextSample() {
    const float sample = amplitude * std::sin(phase);

    // Advance by one sample's worth of the cycle
    phase += twoPi * frequency / static_cast<float>(sampleRate);

    // Wrap around after a full cycle so phase stays small and precise
    if (phase >= twoPi) {
        phase -= twoPi;
    }

    return sample;
}