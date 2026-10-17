//
// Created by Eric El-Chabab on 2026-10-15.
//

#include "Note.h"
#include <cmath>
#include <algorithm>

namespace {
    constexpr int REFERENCE_NOTE = 69;  // A4
    constexpr float REFERENCE_FREQUENCY = 440.0f;  // 440 Hz
    constexpr float NOTES_PER_OCTAVE = 12.0f;
    constexpr int MIN_NOTE = 0;
    constexpr int MAX_NOTE = 127;
}

float noteToFrequency(int note) {
    const int clampedNote = std::clamp(note, MIN_NOTE, MAX_NOTE);

    return REFERENCE_FREQUENCY * std::pow(2.0f, (clampedNote - REFERENCE_NOTE) / NOTES_PER_OCTAVE);
}
