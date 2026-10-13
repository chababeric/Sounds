//
// Created by Eric El-Chabab on 2026-10-12.
//

#include "Voice.h"

namespace {
    constexpr float DEFAULT_AMPLITUDE = 0.5f;
    constexpr float DEFAULT_FREQUENCY = 440.0f;
}

Voice::Voice(uint32_t sampleRate, const EnvelopeSettings& settings)
    : oscillator(DEFAULT_AMPLITUDE, DEFAULT_FREQUENCY, sampleRate, Waveform::Sine),
      envelope(settings.attackTime, settings.decayTime, settings.sustainLevel, settings.releaseTime, sampleRate) {}

float Voice::getNextSample() {
    return oscillator.getNextSample() * envelope.getNextLevel();
}

void Voice::noteOn() {
    envelope.noteOn();
}

void Voice::noteOff() {
    envelope.noteOff();
}

void Voice::setFrequency(float frequency) {
    oscillator.setFrequency(frequency);
}

void Voice::setAmplitude(float amplitude) {
    oscillator.setAmplitude(amplitude);
}

void Voice::setWaveform(Waveform waveform) {
    oscillator.setWaveform(waveform);
}

float Voice::getFrequency() const {
    return oscillator.getFrequency();
}

float Voice::getAmplitude() const {
    return oscillator.getAmplitude();
}

Waveform Voice::getWaveform() const {
    return oscillator.getWaveform();
}

void Voice::setAttackTime(float attackTime) {
    envelope.setAttackTime(attackTime);
}

void Voice::setDecayTime(float decayTime) {
    envelope.setDecayTime(decayTime);
}

void Voice::setSustainLevel(float sustainLevel) {
    envelope.setSustainLevel(sustainLevel);
}

void Voice::setReleaseTime(float releaseTime) {
    envelope.setReleaseTime(releaseTime);
}
