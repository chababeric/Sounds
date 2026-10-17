//
// Created by Eric El-Chabab on 2026-10-01.
//

#include <cmath>
#include <numbers>
#include <cstdint>
#include <atomic>
#include <algorithm>
#include "synth/Oscillator.h"

namespace {
    constexpr float twoPi = 2.0f * std::numbers::pi_v<float>;
    constexpr float MIN_FREQUENCY = 20.0f;
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
    const WaveShape* currentShape = this->shape.load();
    const float x = phase / twoPi;

    const float value = currentShape->valueAt(x);

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
    const float clampFrequency = std::clamp(frequency, MIN_FREQUENCY, static_cast<float>(this->sampleRate)  / 2.0f);
    this->frequency.store(clampFrequency);
}

void Oscillator::setWaveform(Waveform waveform) {
    shape.store(&WaveShape::shapeFor(waveform));
}

float Oscillator::getAmplitude() const {
    return this->amplitude.load();
}

float Oscillator::getFrequency() const {
    return this->frequency.load();
}

Waveform Oscillator::getWaveform() const {
    return this->waveform.load();
}
