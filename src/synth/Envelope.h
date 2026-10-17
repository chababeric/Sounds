//
// Created by Eric El-Chabab on 2026-10-08.
//

#ifndef SOUNDS_ENVELOPE_H
#define SOUNDS_ENVELOPE_H

#include <cstdint>
#include <atomic>

// Settings for an ADSR envelope.
// Every field has a default, so you only need to set the ones you want to change.
struct EnvelopeSettings {
    float attackTime = 0.01f;   // Seconds to rise from 0 to full volume
    float decayTime = 0.1f;     // Seconds to fall from full volume to the sustain level
    float sustainLevel = 0.8f;  // Volume while the note is held, from 0.0 to 1.0 (a level, not a time)
    float releaseTime = 0.3f;   // Seconds to fade to silence after the note is released
};

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

    // Requests the note to start (key pressed). Safe to call from any thread.
    // The audio thread starts the attack at its next sample, from the current level.
    void noteOn();

    // Requests the note to stop (key released). Safe to call from any thread.
    // The audio thread starts the release at its next sample, from the current level.
    void noteOff();

    // True when the envelope is silent and no note has been requested:
    // the voice using it is free for a new note.
    bool isIdle() const;

    // Advances the envelope by one sample and returns the level to apply.
    // Must be called exactly once per sample.
    float getNextLevel();

    // Settings. Times are in seconds (0 to 10), sustainLevel is from 0.0 to 1.0.
    // Safe to call from any thread. Changes take effect the next time a stage starts.
    void setAttackTime(float attackTime);
    void setDecayTime(float decayTime);
    void setSustainLevel(float sustainLevel);
    void setReleaseTime(float releaseTime);

private:
    // --- Settings: written by the main thread (setters), read by the audio thread ---

    // How long the level takes to rise from 0 to 1.0, in seconds.
    std::atomic<float> attackTime;

    // How long the level takes to fall from 1.0 to the sustain level, in seconds.
    std::atomic<float> decayTime;

    // Level held while the note is on, from 0.0 to 1.0. (A level, not a time!)
    std::atomic<float> sustainLevel;

    // How long the level takes to fall to 0 after the note is released, in seconds.
    std::atomic<float> releaseTime;

    // Samples per second (e.g. 44,100). Converts times in seconds into numbers of samples.
    uint32_t sampleRate;


    // --- State: only ever touched by the audio thread, so no atomics needed ---

    // The volume right now, from 0.0 to 1.0. Changes by currentStep every sample.
    float currentLevel = 0.0f;

    // Which phase of the note we're in.
    // Written only by the audio thread; atomic because isIdle() reads it from the main thread.
    std::atomic<Stage> currentStage{Stage::Idle};

    // How much currentLevel changes per sample in this stage.
    // Positive when rising, negative when falling, 0 when holding still.
    // Computed once when entering a stage: (target - current) / (time * sampleRate).
    float currentStep = 0.0f;

    // What the main thread wants: true = note on, false = note off.
    // Written by noteOn() / noteOff() on the main thread, read by the audio thread.
    std::atomic<bool> noteRequested{false};

    // What the audio thread last acted on. When it differs from noteRequested,
    // the audio thread switches to Attack or Release. Audio thread only.
    bool noteIsOn = false;

    // Moves to a new stage, and computes its step from the current level.
    // Looks up the target level and time for the stage from the settings above.
    void switchStage(Stage newStage);
};

#endif // SOUNDS_ENVELOPE_H
