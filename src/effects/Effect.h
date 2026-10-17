//
// Created by Eric El-Chabab on 2026-10-17.
//

#ifndef SOUNDS_EFFECT_H
#define SOUNDS_EFFECT_H

#include <atomic>

// An audio effect: takes a sound in and gives a modified sound out (distortion, delay, filter...).
// Effects can be chained: the output of one becomes the input of the next.
// Unlike wave shapes, most effects have memory (a delay remembers past samples),
// so each effect is its own object with its own state, never shared.
class Effect {
public:
    // Processes one sample and returns the result.
    // Must be called exactly once per sample, in order: the effect's memory depends on it.
    // Runs on the audio thread, so no allocation, locks or waiting inside.
    virtual float process(float sample) = 0;

    virtual ~Effect() = default;

    // Turns the effect on (true) or bypasses it (false). Safe to call from any thread.
    void setEnabled(bool enabled) {this->enabled.store(enabled);}

    // True if the effect is applied, false if it's bypassed.
    bool isEnabled() const { return this->enabled.load(); }

private:
    // Whether the effect is applied. Shared by every effect through inheritance.
    // Written by the main thread (setEnabled), read by the audio thread (in EffectChain).
    std::atomic<bool> enabled{true};
};

#endif //SOUNDS_EFFECT_H
