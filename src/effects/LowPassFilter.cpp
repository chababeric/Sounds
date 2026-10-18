//
// Created by Eric El-Chabab on 2026-10-18.
//

#include <algorithm>
#include <numbers>
#include <cmath>
#include "effects/LowPassFilter.h"

namespace {
    constexpr float twoPi = 2.0f * std::numbers::pi_v<float>;
    constexpr float MIN_CUTOFF = 20.0f;
}

LowPassFilter::LowPassFilter(uint32_t sampleRate, float cutoff)
    : sampleRate(sampleRate) {
    setCutoff(cutoff);
}

float LowPassFilter::process(float sample) {
    const float currentA = this->a.load();
    previousOutput = previousOutput + currentA * (sample - previousOutput);
    return previousOutput;
}

void LowPassFilter::setCutoff(float cutoff) {
    const float currentCutoff = std::clamp(cutoff, MIN_CUTOFF, static_cast<float>(sampleRate) / 2.0f);
    this->cutoff.store(currentCutoff);

    this->a.store(1.0f - std::exp(-twoPi * currentCutoff / static_cast<float>(sampleRate)));
}

float LowPassFilter::getCutoff() const {
    return this->cutoff.load();
}
