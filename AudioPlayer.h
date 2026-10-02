//
// Created by Eric El-Chabab on 2026-10-02.
//

#ifndef SOUNDS_AUDIOPLAYER_H
#define SOUNDS_AUDIOPLAYER_H
#include <cstdint>
#include "miniaudio.h"
#include "Oscillator.h"

// Plays an Oscillator live through the default audio output.
// The sound card calls back into this class whenever it needs more samples.
class AudioPlayer {
public:
    AudioPlayer(Oscillator& oscilator, uint32_t sampleRate, uint16_t channelCount);
    ~AudioPlayer();

    // miniaudio keeps a pointer to this object, so it must never be copied
    AudioPlayer(const AudioPlayer&) = delete;
    AudioPlayer& operator=(const AudioPlayer&) = delete;

    void start();
    void stop();

private:
    // Called by miniaudio on the audio thread each time the sound card needs more frames.
    // Must be static: miniaudio is a C library and can't call a member function directly.
    static void dataCallback(ma_device* device, void* output, const void* input, ma_uint32 frameCount);

    // Fills the output buffer with samples. Runs on the audio thread, so:
    // no allocation, no locks, no printing, no waiting.
    void fillBuffer(float* output, ma_uint32 frameCount);

    // The sound source. A reference: the player uses it but doesn't own it.
    Oscillator& oscillator;

    // The open audio device (miniaudio's handle to the sound card).
    ma_device device{};
};


#endif //SOUNDS_AUDIOPLAYER_H
