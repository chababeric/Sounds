//
// Created by Eric El-Chabab on 2026-10-08.
//

#include "Envelope.h"
#include <algorithm>

namespace {
    // Longest time allowed for attack, decay and release, in seconds
    constexpr float MAX_STAGE_TIME = 10.0f;
}

Envelope::Envelope(float attackTime, float decayTime, float sustainLevel, float releaseTime, uint32_t sampleRate)
    : sampleRate(sampleRate) {
    setAttackTime(attackTime);
    setDecayTime(decayTime);
    setSustainLevel(sustainLevel);
    setReleaseTime(releaseTime);
}

void Envelope::switchStage(Stage newStage) {
    currentStage = newStage;

    // 1. Find where this stage goes, and how long it takes
    float targetLevel = 0.0f;
    float time = 0.0f;

    switch (newStage) {
        case Stage::Idle:
        case Stage::Sustain:
            // These stages hold still: no movement, no formula
            currentStep = 0.0f;
            return;
        case Stage::Attack:
            targetLevel = 1.0f;
            time = attackTime.load();
            break;
        case Stage::Decay:
            targetLevel = sustainLevel.load();
            time = decayTime.load();
            break;
        case Stage::Release:
            targetLevel = 0.0f;
            time = releaseTime.load();
            break;
    }

    // 2. A stage that takes no time: jump straight to the target
    if (time <= 0.0f) {
        currentLevel = targetLevel;
        currentStep = 0.0f;
        return;
    }

    // 3. Step per sample = distance / number of samples
    const float sampleCount = time * static_cast<float>(sampleRate);
    currentStep = (targetLevel - currentLevel) / sampleCount;
}

void Envelope::noteOn() {
    noteRequested.store(true);
}

void Envelope::noteOff() {
    noteRequested.store(false);
}

float Envelope::getNextLevel() {
    const bool isNoteRequested = noteRequested.load();
    const float currentSustainLevel = this->sustainLevel.load();

    if (isNoteRequested != noteIsOn) {
        noteIsOn = isNoteRequested;
        if (noteIsOn) {
            switchStage(Stage::Attack);
        }
        else {
            switchStage(Stage::Release);
        }
    }

    this->currentLevel += this->currentStep;
    switch (this->currentStage) {
        case Stage::Attack:
            if (this->currentLevel >= 1.0f) {
                this->currentLevel = 1.0f;
                switchStage(Stage::Decay);
            }
            break;
        case Stage::Decay:
            if (this->currentLevel <= currentSustainLevel) {
                this->currentLevel = currentSustainLevel;
                switchStage(Stage::Sustain);
            }
            break;
        case Stage::Release:
            if (this->currentLevel <= 0.0f) {
                this->currentLevel = 0.0f;
                switchStage(Stage::Idle);
            }
            break;
        case Stage::Sustain:
        case Stage::Idle:
            break;
    }
    return currentLevel;
}

void Envelope::setAttackTime(float attackTime) {
    this->attackTime = std::clamp(attackTime, 0.0f, MAX_STAGE_TIME);
}

void Envelope::setDecayTime(float decayTime) {
    this->decayTime = std::clamp(decayTime, 0.0f, MAX_STAGE_TIME);
}

void Envelope::setSustainLevel(float sustainLevel) {
    this->sustainLevel = std::clamp(sustainLevel, 0.0f, 1.0f);
}

void Envelope::setReleaseTime(float releaseTime) {
    this->releaseTime = std::clamp(releaseTime, 0.0f, MAX_STAGE_TIME);
}
