//
// Created by Eric El-Chabab on 2026-10-18.
//

#ifndef SOUNDS_SAMPLEPLAYER_H
#define SOUNDS_SAMPLEPLAYER_H

#include <cstddef>
#include <atomic>
#include "audio/AudioBuffer.h"

// Plays a loaded AudioBuffer once from start to end (a "one-shot" sample).
// Multichannel files are mixed down to mono (channels averaged).
//
// Threading: trigger() and isPlaying() can be called from the main thread;
// getNextSample() is called by the audio thread only.
class SamplePlayer {
public:
    // Takes the audio by value and moves it in, so the caller can std::move a buffer without copying.
    explicit SamplePlayer(AudioBuffer sample);

    // Asks the audio thread to (re)start playing from the beginning.
    // Only raises a flag; the audio thread does the restart on its next sample.
    void trigger();

    // The next sample of the audio, or 0.0f when not playing. Audio thread only.
    float getNextSample();

    // True while the sample is playing.
    bool isPlaying() const;

    // Playback speed: 1 = normal, 2 = one octave up, 0.5 = one octave down.
    // Clamped to 0.25..4 (two octaves down to two up). Safe to call from the main thread.
    void setSpeed(float speed);

private:
    // The audio being played. Never changes after construction.
    AudioBuffer sample;

    // Position in frames, advanced by "speed" each sample, so it can be fractional. Audio thread only.
    double position{0.0};

    // Written by the audio thread, read by the main thread in isPlaying().
    std::atomic<bool> playing{false};

    // Set by trigger() on the main thread, taken by getNextSample() on the audio thread.
    std::atomic<bool> triggerRequested{false};

    // Number of frames in the sample. Computed once; must be declared after "sample".
    const size_t frameCount;

    // Playback speed. Set by the main thread, read by the audio thread.
    std::atomic<float> speed{1.0f};
};


#endif //SOUNDS_SAMPLEPLAYER_H
