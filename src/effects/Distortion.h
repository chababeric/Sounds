//
// Created by Eric El-Chabab on 2026-10-17.
//

#ifndef SOUNDS_DISTORTION_H
#define SOUNDS_DISTORTION_H

#include <atomic>
#include "effects/Effect.h"

// Distortion by soft clipping: pushes the signal into a tanh curve,
// which rounds off the loud peaks instead of chopping them flat.
// Quiet parts stay almost clean; loud parts get warm and gritty.
// The output always stays between -1.0 and +1.0.
class Distortion : public Effect {
public:
    // drive: how hard the signal is pushed into the curve, from 1 to 20 (clamped).
    explicit Distortion(float drive);

    // Returns tanh(drive x sample). Runs on the audio thread.
    float process(float sample) override;

    // 1 = subtle warmth, around 5 = clearly distorted, 20 = heavy fuzz.
    // Safe to call from any thread while playing.
    void setDrive(float drive);

    float getDrive() const;

private:
    // How hard the signal is pushed into the curve (1 to 20).
    // Written by the main thread (setDrive), read by the audio thread (process).
    std::atomic<float> drive;
};

#endif //SOUNDS_DISTORTION_H
