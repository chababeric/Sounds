//
// Created by Eric El-Chabab on 2026-10-30.
//

#include <utility>
#include <vector>
#include <cmath>
#include <algorithm>
#include "editor/Edits.h"

void Edits::reverse(AudioBuffer& buffer) {
    std::vector<float>& samples = buffer.getSamples();    // reference: edit the real samples
    const uint16_t channelCount = buffer.getChannelCount();
    const size_t frameCount = samples.size() / channelCount;

    // Swap the first frame with the last, the second with the second-to-last... up to the middle
    for (size_t i = 0; i < frameCount / 2; i++) {
        const size_t frontFrame = i;
        const size_t backFrame = frameCount - 1 - i;

        // Swap the two frames channel by channel, so L stays L and R stays R
        for (uint16_t c = 0; c < channelCount; c++) {
            std::swap(samples[frontFrame * channelCount + c],
                      samples[backFrame * channelCount + c]
            );
        }
    }
}

float Edits::normalize(AudioBuffer& buffer, float targetPeak) {
    std::vector<float>& samples = buffer.getSamples();
    if (samples.empty()) { return 1.0f; }

    const auto loudest = std::ranges::max_element(samples, {}, [](float x) { return std::abs(x); });
    const float peak = std::abs(*loudest);
    if (peak == 0.0f) { return 1.0f; }    // silence: nothing to scale (and avoids dividing by zero)

    const float gain = targetPeak / peak;
    applyGain(buffer, gain);

    return gain;
}

void Edits::applyGain(AudioBuffer &buffer, float gain) {
    std::vector<float>& samples = buffer.getSamples();

    for (float& sample : samples) {
        sample *= gain;
    }
}

void Edits::fadeIn(AudioBuffer& buffer, size_t fadeFrames) {
    std::vector<float>& samples = buffer.getSamples();
    const uint16_t channelCount = buffer.getChannelCount();
    const size_t frameCount = samples.size() / channelCount;

    fadeFrames = std::min(fadeFrames, frameCount);    // never longer than the file
    if (fadeFrames == 0) return;                      // nothing to fade (and avoids dividing by 0)

    for (size_t i = 0; i < fadeFrames; i++) {
        // Rises from 0 (silent) at the first frame toward 1 (full volume) at the end of the fade
        const float gain = static_cast<float>(i) / static_cast<float>(fadeFrames);

        // Same gain for every channel of this frame
        for (uint16_t c = 0; c < channelCount; c++) {
            samples[i * channelCount + c] *= gain;
        }
    }
}
