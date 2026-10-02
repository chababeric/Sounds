//
// Created by Eric El-Chabab on 2026-10-02.
//

#include "AudioPlayer.h"
#include <stdexcept>

AudioPlayer::AudioPlayer(Oscillator& oscillator, uint32_t sampleRate, uint16_t channelCount)
    : oscillator(oscillator) {
    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format = ma_format_f32;      // 32-bit floats, same as our oscillator
    config.playback.channels = channelCount;
    config.sampleRate = sampleRate;
    config.dataCallback = dataCallback;
    config.pUserData = this;                     // miniaudio hands this back to dataCallback

    if (ma_device_init(nullptr, &config, &device) != MA_SUCCESS) {
        throw std::runtime_error("Failed to open the audio device");
    }
}

AudioPlayer::~AudioPlayer() {
    ma_device_uninit(&device);                   // Stops playback and closes the device
}

void AudioPlayer::start() {
    if (ma_device_start(&device) != MA_SUCCESS) {
        throw std::runtime_error("Failed to start the audio device");
    }
}

void AudioPlayer::stop() {
    ma_device_stop(&device);
}

void AudioPlayer::dataCallback(ma_device* device, void* output, const void* /*input*/, ma_uint32 frameCount) {
    // Get our AudioPlayer object back from the pointer we stored in pUserData
    auto* player = static_cast<AudioPlayer*>(device->pUserData);
    player->fillBuffer(static_cast<float*>(output), frameCount);
}

void AudioPlayer::fillBuffer(float* output, ma_uint32 frameCount) {
    const ma_uint32 channelCount = device.playback.channels;

    for (ma_uint32 frame = 0; frame < frameCount; ++frame) {
        const float sample = oscillator.getNextSample();

        // Same sample on every channel (interleaved: left, right, left, right...)
        for (ma_uint32 channel = 0; channel < channelCount; ++channel) {
            output[frame * channelCount + channel] = sample;
        }
    }
}