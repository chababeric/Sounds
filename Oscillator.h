//
// Created by Eric El-Chabab on 2026-10-01.
//

#ifndef SOUNDS_OSCILLATOR_H
#define SOUNDS_OSCILLATOR_H


class Oscillator {
public:
    Oscillator(float amplitude, float frequency, float sampleRate);

    float getNextSample();

private:
    float amplitude;
    float frequency;
    float sampleRate;
    float phase = 0.0f;
};


#endif //SOUNDS_OSCILLATOR_H
