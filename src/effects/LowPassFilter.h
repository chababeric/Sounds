//
// Created by Eric El-Chabab on 2026-10-18.
//

#ifndef SOUNDS_LOWPASSFILTER_H
#define SOUNDS_LOWPASSFILTER_H

#include <cstdint>
#include <atomic>
#include "effects/Effect.h"

// Low-pass filter: lets low frequencies through and softens the high ones,
// making the sound darker and more muffled (like music heard through a wall).
// One-pole design: each sample, the output moves part of the way toward the input.
// Fast changes (high frequencies) can't be followed, so they come out quieter;
// slow changes (low frequencies) pass almost unchanged.
// Its only memory is one number: the previous output.
class LowPassFilter : public Effect {
public:
    // cutoff: in Hz, see setCutoff.
    LowPassFilter(uint32_t sampleRate, float cutoff);

    // Returns the filtered sample. Runs on the audio thread.
    float process(float sample) override;

    // Frequency above which sounds get softer, in Hz, from 20 to half the sample rate.
    // Lower = darker (e.g. 200 is very muffled), higher = brighter (20000 is almost no effect).
    // Safe to call from any thread while playing.
    void setCutoff(float cutoff);

    float getCutoff() const;

private:
    // Samples per second. Needed to turn the cutoff into the coefficient a.
    uint32_t sampleRate;

    // --- Settings: written by the main thread (setCutoff), read by the audio thread ---

    // The cutoff frequency in Hz, kept for display.
    std::atomic<float> cutoff{0.0f};

    // How far the output moves toward the input each sample, from 0 to 1.
    // Computed from the cutoff in setCutoff: a = 1 - e^(-2π × cutoff / sampleRate).
    // Close to 1 = follows the input closely (bright); close to 0 = follows slowly (dark).
    std::atomic<float> a{1.0f};

    // --- State: audio thread only, so not atomic ---

    // The previous output: the filter's memory.
    float previousOutput = 0.0f;
};


#endif //SOUNDS_LOWPASSFILTER_H
