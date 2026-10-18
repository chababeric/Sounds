//
// Created by Eric El-Chabab on 2026-10-17.
//

#ifndef SOUNDS_DELAY_H
#define SOUNDS_DELAY_H

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "effects/Effect.h"

// Echo effect: plays the sound back a moment later, mixed with the original.
// Remembers the recent past in a circular buffer: a fixed-size array where a write
// position moves forward one slot per sample and wraps back to 0 at the end.
// The slot about to be overwritten always holds the oldest sample.
class Delay : public Effect {
public:
    // maximumDelayTime: the longest delay possible, in seconds. The buffer is sized
    // for it once, here, so nothing is allocated while playing.
    Delay(uint32_t sampleRate, float maximumDelayTime);

    // Returns the original sample mixed with the echo. Runs on the audio thread.
    float process(float sample) override;

    // Settings. Safe to call from any thread while playing.

    // Time until the echo, in seconds (clamped to the maximum given to the constructor).
    void setDelayTime(float delayTime);

    // How much of each echo is fed back to create the next one, from 0 to 0.95.
    // 0 = a single echo; higher = more, longer-lasting echoes. Must stay below 1,
    // or the echoes would grow louder forever (runaway feedback).
    void setFeedback(float feedback);

    // Balance between the original sound and the echoes (a crossfade), from 0 to 1:
    // 0 = only the original, 0.5 = half and half, 1 = only the echoes.
    void setMix(float mix);

    float getDelayTime() const;
    float getFeedback() const;
    float getMix() const;

private:
    // Samples per second. Converts delay times between seconds and samples.
    uint32_t sampleRate;

    // The circular buffer: the most recent samples, oldest overwritten first.
    // Sized once in the constructor. Audio thread only after that.
    std::vector<float> buffer;

    // Where the next sample will be written. Moves forward one slot per sample,
    // wrapping to 0 at the end. Audio thread only, so not atomic.
    size_t writePosition = 0;

    // --- Settings: written by the main thread (setters), read by the audio thread ---

    // How many samples back the echo is read from.
    // Stored in samples (not seconds) so process() doesn't have to convert every sample.
    std::atomic<size_t> delaySamples{1};

    // Echo feedback, from 0 to 0.95.
    std::atomic<float> feedback{0.0f};

    // Crossfade between original (0) and echoes (1).
    std::atomic<float> mix{0.0f};
};

#endif //SOUNDS_DELAY_H
