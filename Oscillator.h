//
// Created by Eric El-Chabab on 2026-10-01.
//

#ifndef SOUNDS_OSCILLATOR_H
#define SOUNDS_OSCILLATOR_H

#include <cstdint>
#include <atomic>
#include "Waveform.h"

class Oscillator {
public:
    Oscillator(float amplitude, float frequency, uint32_t sampleRate, Waveform waveform = Waveform::Sine);

    float getNextSample();
    void setAmplitude(float amplitude);
    void setFrequency(float frequency);
    void setWaveform(Waveform waveform);
    float getAmplitude() const;
    float getFrequency() const;
    Waveform getWaveform() const;

private:
    // Volume of the wave, from 0.0 (silent) to 1.0 (maximum).
    // The wave swings between -amplitude and +amplitude.
    std::atomic<float> amplitude;

    // Pitch of the tone, in Hz (cycles per second).
    // 440 Hz = the note A. Higher number = higher pitch.
    std::atomic<float> frequency;

    // How many samples are produced per second (e.g. 44,100).
    // Must match the AudioBuffer the samples go into.
    uint32_t sampleRate;

    // Where we are inside the current cycle of the wave, in radians.
    // Goes from 0 to 2π, then wraps back to 0 for the next cycle.
    float phase = 0.0f;

    // Shape of the wave: sine, square, saw or triangle
    std::atomic<Waveform> waveform;
};


#endif //SOUNDS_OSCILLATOR_H
