//
// Created by Eric El-Chabab on 2026-10-01.
//

#ifndef SOUNDS_OSCILLATOR_H
#define SOUNDS_OSCILLATOR_H

#include <cstdint>

class Oscillator {
public:
    Oscillator(float amplitude, float frequency, uint32_t sampleRate);

    float getNextSample();

private:
    // Volume of the wave, from 0.0 (silent) to 1.0 (maximum).
    // The wave swings between -amplitude and +amplitude.
    float amplitude;

    // Pitch of the tone, in Hz (cycles per second).
    // 440 Hz = the note A. Higher number = higher pitch.
    float frequency;

    // How many samples are produced per second (e.g. 44,100).
    // Must match the AudioBuffer the samples go into.
    uint32_t sampleRate;

    // Where we are inside the current cycle of the wave, in radians.
    // Goes from 0 to 2π, then wraps back to 0 for the next cycle.
    float phase = 0.0f;
};


#endif //SOUNDS_OSCILLATOR_H
