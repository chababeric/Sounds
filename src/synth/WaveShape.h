//
// Created by Eric El-Chabab on 2026-10-15.
//

#ifndef SOUNDS_WAVESHAPE_H
#define SOUNDS_WAVESHAPE_H

#include "synth/Waveform.h"

// The shape of one cycle of a wave. Each subclass is one waveform.
// Shapes have no state: the oscillator keeps track of time, and asks the shape
// for the value at its current position in the cycle.
class WaveShape {
public:
    // Returns the wave's value at a given position in one cycle.
    // position: from 0.0 (start of the cycle) to just under 1.0 (end of the cycle).
    // Returns a value from -1.0 to +1.0.
    virtual float valueAt(float position) const = 0;

    virtual ~WaveShape() = default;

    // Returns the shared shape object for a waveform.
    // The four shapes are created once and live for the whole program,
    // so the returned reference is always safe to keep.
    static const WaveShape& shapeFor(Waveform waveform);
};

// Smooth curve: pure, soft tone.
class SineShape : public WaveShape {
public:
    float valueAt(float position) const override;
};

// +1 for the first half of the cycle, -1 for the second: hollow, buzzy tone.
class SquareShape : public WaveShape {
public:
    float valueAt(float position) const override;
};

// Straight lines up and down: soft, mellow tone.
class TriangleShape : public WaveShape {
public:
    float valueAt(float position) const override;
};

// Ramp from -1 up to +1, then an instant drop: bright, harsh tone.
class SawShape : public WaveShape {
public:
    float valueAt(float position) const override;
};

#endif // SOUNDS_WAVESHAPE_H
