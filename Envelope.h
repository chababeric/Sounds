//
// Created by Eric El-Chabab on 2026-10-08.
//

#ifndef SOUNDS_ENVELOPE_H
#define SOUNDS_ENVELOPE_H

#include <cstdint>

// Shapes the volume of a note over time (ADSR: Attack, Decay, Sustain, Release).
// Produces one level per sample, from 0.0 (silent) to 1.0 (full volume),
// which is multiplied with the oscillator's output.
class Envelope {
public:
    // The phases of a note's life, in order.
    enum class Stage {
        Idle,       // No note playing: level stays at 0
        Attack,     // Note just started: level rises from its current value to 1.0
        Decay,      // Level falls from 1.0 to the sustain level
        Sustain,    // Note held: level stays at the sustain level
        Release     // Note released: level falls from its current value to 0
    };

    // Times are in seconds, sustainLevel is from 0.0 to 1.0.
    Envelope(float attackTime, float decayTime, float sustainLevel, float releaseTime, uint32_t sampleRate);

    // Starts the note (key pressed): goes to Attack from the current level.
    void noteOn();

    // Ends the note (key released): goes to Release from the current level.
    void noteOff();

    // Advances the envelope by one sample and returns the level to apply.
    // Must be called exactly once per sample.
    float getNextLevel();

    void setAttackTime(float attackTime);
    void setDecayTime(float decayTime);
    void setSustainLevel(float sustainLevel);
    void setReleaseTime(float releaseTime);

private:
    // How long the level takes to rise from 0 to 1.0, in seconds.
    float attackTime;

    // How long the level takes to fall from 1.0 to the sustain level, in seconds.
    float decayTime;

    // Level held while the note is on, from 0.0 to 1.0. (A level, not a time!)
    float sustainLevel;

    // How long the level takes to fall to 0 after the note is released, in seconds.
    float releaseTime;

    // Samples per second (e.g. 44,100). Converts times in seconds into numbers of samples.
    uint32_t sampleRate;

    // The volume right now, from 0.0 to 1.0. Changes by currentStep every sample.
    float currentLevel = 0.0f;

    // Which phase of the note we're in.
    Stage currentStage = Stage::Idle;

    // How much currentLevel changes per sample in this stage.
    // Positive when rising, negative when falling, 0 when holding still.
    // Computed once when entering a stage: (target - current) / (time * sampleRate).
    float currentStep = 0.0f;

    // Moves to a new stage, and computes its step from the current level.
    // Looks up the target level and time for the stage from the settings above.
    void switchStage(Stage newStage);
};

#endif // SOUNDS_ENVELOPE_H
