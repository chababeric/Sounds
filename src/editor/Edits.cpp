//
// Created by Eric El-Chabab on 2026-10-30.
//

#include <utility>
#include <vector>
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
                      samples[backFrame * channelCount + c]);
        }
    }
}
