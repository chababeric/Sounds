//
// Created by Eric El-Chabab on 2026-10-15.
//

#ifndef SOUNDS_SYNTH_H
#define SOUNDS_SYNTH_H

#include <memory>
#include <vector>
#include <cstdint>
#include <atomic>
#include "Voice.h"
#include "Waveform.h"

// A polyphonic synthesizer: a fixed pool of voices that can play several notes at once.
class Synth {
public:
    // How many notes can sound at the same time.
    static constexpr int MAX_VOICES = 8;

    // Creates all the voices up front, so nothing is allocated while playing.
    explicit Synth(uint32_t sampleRate);

    // Starts a MIDI note (0 to 127) on a free voice.
    // Returns false if all voices are busy (the note isn't played).
    void noteOn(int note);

    // Releases the voice holding this note, if any.
    void noteOff(int note);

    // The mix of all voices for one sample. Must be called exactly once per sample.
    float getNextSample();

    // Shape of the wave for every voice: sine, square, saw or triangle.
    void setWaveform(Waveform waveform);
    Waveform getWaveform() const;

    // How many voices are currently making sound (including notes fading out).
    int getActiveVoiceCount() const;

private:
    // Volume of the whole mix, applied after summing the voices. Keeps chords from clipping.
    std::atomic<float> masterGain{0.25f};

    Voice* findOldestVoice(bool released);

    // The voice pool. unique_ptr because Voice can't be copied or moved (it contains atomics);
    // the vector only ever moves the pointers, and deletes the voices automatically.
    std::vector<std::unique_ptr<Voice>> voices;

    uint64_t noteCounter = 0;
};

#endif //SOUNDS_SYNTH_H
