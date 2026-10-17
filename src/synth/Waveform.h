//
// Created by Eric El-Chabab on 2026-10-04.
//

#ifndef SOUNDS_WAVEFORM_H
#define SOUNDS_WAVEFORM_H

// The shape of the wave an oscillator produces.
// All shapes have the same pitch at a given frequency;
// the shape changes the timbre (the "color" of the sound).
enum class Waveform {
    // Smooth curve, like a pendulum swinging.
    // Sound: pure and soft, no harshness. A tuning fork or a flute-like tone.
    Sine,

    // +1 for the first half of the cycle, -1 for the second half.
    // Sound: hollow and buzzy, like old video game consoles.
    Square,

    // Straight ramp from -1 up to +1, then an instant drop back to -1.
    // Sound: bright and harsh, the classic synthesizer sound (strings, brass).
    Saw,

    // Straight line up from -1 to +1, then straight line back down.
    // Sound: soft and mellow, between a sine and a square.
    Triangle
};

#endif //SOUNDS_WAVEFORM_H
