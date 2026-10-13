//
// Created by Eric El-Chabab on 2026-10-12.
//

#ifndef SOUNDS_VOICE_H
#define SOUNDS_VOICE_H

#include <cstdint>
#include "Envelope.h"
#include "Oscillator.h"
#include "Waveform.h"

// One playable note: an oscillator (the sound) shaped by an envelope (its volume over time).
// Call getNextSample() once per sample to get the final output.
class Voice {
public:
    // Creates a silent voice (no note playing) with a default sound: 440 Hz sine wave.
    // Use the setters to change the sound, and noteOn() to start playing.
    Voice(uint32_t sampleRate, const EnvelopeSettings& settings = {});

    // Produces the next sample: oscillator output x envelope level.
    // Must be called exactly once per sample.
    float getNextSample();

    // --- Notes ---

    // Starts the note (key pressed): the envelope begins its attack.
    void noteOn();

    // Ends the note (key released): the envelope begins its release.
    void noteOff();

    // --- Sound settings (passed to the oscillator) ---

    // Pitch in Hz (20 to half the sample rate).
    void setFrequency(float frequency);

    // Volume from 0.0 (silent) to 1.0 (maximum).
    void setAmplitude(float amplitude);

    // Shape of the wave: sine, square, saw or triangle.
    void setWaveform(Waveform waveform);

    float getFrequency() const;
    float getAmplitude() const;
    Waveform getWaveform() const;

    // --- Envelope settings (passed to the envelope) ---
    // Changes take effect the next time the envelope enters a stage.

    // Time to rise from silence to full volume, in seconds.
    void setAttackTime(float attackTime);

    // Time to fall from full volume to the sustain level, in seconds.
    void setDecayTime(float decayTime);

    // Volume held while the note is on, from 0.0 to 1.0. (A level, not a time!)
    void setSustainLevel(float sustainLevel);

    // Time to fade to silence after the note is released, in seconds.
    void setReleaseTime(float releaseTime);

private:
    // Generates the raw waveform.
    Oscillator oscillator;

    // Shapes the waveform's volume over time.
    Envelope envelope;
};

#endif // SOUNDS_VOICE_H
