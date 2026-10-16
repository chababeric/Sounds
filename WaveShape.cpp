//
// Created by Eric El-Chabab on 2026-10-15.
//

#include "WaveShape.h"
#include <cmath>
#include <numbers>

namespace {
    constexpr float twoPi = 2.0f * std::numbers::pi_v<float>;
}

float SineShape::valueAt(float position) const {
    return std::sin(twoPi * position);
}

float SquareShape::valueAt(float position) const {
    return (position < 0.5f) ? 1.0f : -1.0f;
}

float SawShape::valueAt(float position) const {
    return (2.0f * position) - 1.0f;
}

float TriangleShape::valueAt(float position) const {
    return 1.0f - 4.0f * std::abs(position - 0.5f);
}

const WaveShape& WaveShape::shapeFor(Waveform waveform) {
    static const SineShape sine;
    static const SquareShape square;
    static const TriangleShape triangle;
    static const SawShape saw;

    switch (waveform) {
        case Waveform::Sine:
            return sine;
        case Waveform::Square:
            return square;
        case Waveform::Saw:
            return saw;
        case Waveform::Triangle:
            return triangle;
    }

    return sine;
}
