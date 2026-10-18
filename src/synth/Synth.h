//
// Created by Eric El-Chabab on 2026-10-15.
//

#ifndef SOUNDS_SYNTH_H
#define SOUNDS_SYNTH_H

#include <memory>
#include <vector>
#include <cstdint>
#include <atomic>
#include "synth/Voice.h"
#include "synth/Waveform.h"
#include "effects/EffectChain.h"
#include "effects/Effect.h"
#include "sampler/SamplePlayer.h"

// A polyphonic synthesizer: a fixed pool of voices that can play several notes at once.
class Synth {
public:
    // How many notes can sound at the same time.
    static constexpr int MAX_VOICES = 8;

    // Creates all the voices up front, so nothing is allocated while playing.
    explicit Synth(uint32_t sampleRate);

    // Starts a MIDI note (0 to 127) on a free voice.
    void noteOn(int note);

    // Releases the voice holding this note, if any.
    void noteOff(int note);

    // The mix of all voices and the sample player for one sample, through the effects.
    // Must be called exactly once per sample.
    float getNextSample();

    // Shape of the wave for every voice: sine, square, saw or triangle.
    void setWaveform(Waveform waveform);
    Waveform getWaveform() const;

    // How many voices are currently making sound (including notes fading out).
    int getActiveVoiceCount() const;

    // Adds an effect to the end of the effect chain, which takes ownership of it.
    // Setup only: call before playback starts.
    void addEffect(std::unique_ptr<Effect> effect);

    // Gives the synth a sample player, mixed in with the voices before the effects.
    // The synth takes ownership; keep a plain pointer to call trigger() later.
    // Setup only: NOT thread-safe, call before playback starts
    // (replacing it while the audio thread uses it would delete it under its feet).
    void setSamplePlayer(std::unique_ptr<SamplePlayer> samplePlayer);

private:
    // Volume of the whole mix, applied after summing the voices. Keeps chords from clipping.
    std::atomic<float> masterGain{0.25f};

    // Finds the active voice with the smallest start order (the oldest note),
    // among released voices (released = true) or held voices (released = false).
    // Returns nullptr if there is none.
    Voice* findOldestVoice(bool released);

    // The voice pool. unique_ptr because Voice can't be copied or moved (it contains atomics);
    // the vector only ever moves the pointers, and deletes the voices automatically.
    std::vector<std::unique_ptr<Voice>> voices;

    // Goes up by one on every noteOn, so each note gets a unique, increasing number.
    // Used to find the oldest voice when stealing. Main thread only.
    uint64_t noteCounter = 0;

    // The effects applied to the mix of all voices, before the master gain.
    EffectChain effectChain;

    // Optional one-shot sample player. nullptr until setSamplePlayer is called.
    std::unique_ptr<SamplePlayer> samplePlayer;
};

#endif //SOUNDS_SYNTH_H
