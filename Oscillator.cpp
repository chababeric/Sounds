//
// Created by Eric El-Chabab on 2026-10-01.
//

#include "Oscillator.h"
#include <cmath>
#include <numbers>
#include <cstdint>
#include <atomic>
#include <algorithm>

namespace {
    constexpr float twoPi = 2.0f * std::numbers::pi_v<float>;
}

Oscillator::Oscillator(float amplitude, float frequency, uint32_t sampleRate, Waveform waveform)
    : sampleRate(sampleRate) {
    setAmplitude(amplitude);
    setFrequency(frequency);
    setWaveform(waveform);
}

float Oscillator::getNextSample() {
    const float currentAmplitude = this->amplitude.load();
    const float currentFrequency = this->frequency.load();
    const Waveform currentWaveform = this->waveform.load();
    const float x = phase / twoPi;
    float value = 0.0f;

    switch (currentWaveform) {
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
    phase += twoPi * currentFrequency / static_cast<float>(sampleRate);

    // Wrap around after a full cycle so phase stays small and precise
    if (phase >= twoPi) {
        phase -= twoPi;
    }

    return value * currentAmplitude;
}

void Oscillator::setAmplitude(float amplitude) {
    const float clampAmplitude = std::clamp(amplitude, 0.0f, 1.0f);
    this->amplitude.store(clampAmplitude);
}

void Oscillator::setFrequency(float frequency) {
    const float clampFrequency = std::clamp(frequency, 0.0f, static_cast<float>(this->sampleRate)  / 2.0f);
    this->frequency.store(clampFrequency);
}

void Oscillator::setWaveform(Waveform waveform) {
    this->waveform.store(waveform);
}

float Oscillator::getAmplitude() {
    return this->amplitude.load();
}

float Oscillator::getFrequency() {
    return this->frequency.load();
}

Waveform Oscillator::getWaveform() {
    return this->waveform.load();
}
