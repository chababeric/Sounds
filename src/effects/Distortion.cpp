//
// Created by Eric El-Chabab on 2026-10-17.
//

#include "Distortion.h"

#include <algorithm>
#include <cmath>

namespace {
    constexpr float MIN_DRIVE = 1.0f;
    constexpr float MAX_DRIVE = 20.0f;
}

Distortion::Distortion(float drive) {
    setDrive(drive);
}

float Distortion::process(float sample) {
    return std::tanh(drive.load() * sample);
}

void Distortion::setDrive(float drive) {
    this->drive.store(std::clamp(drive, MIN_DRIVE, MAX_DRIVE));
}

float Distortion::getDrive() const {
    return this->drive.load();
}
