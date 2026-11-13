//
// Created by Eric El-Chabab on 2026-11-13.
//

#include "editor/FadeInCommand.h"
#include "editor/Edits.h"
#include <algorithm>

FadeInCommand::FadeInCommand(size_t fadeFrames)
    : fadeFrames(fadeFrames) {}

void FadeInCommand::apply(AudioBuffer &buffer) {
    const std::vector<float>& samples = buffer.getSamples();
    const uint16_t channelCount = buffer.getChannelCount();
    const size_t frameCount = samples.size() / channelCount;

    // Save only the samples the fade will change (same clamp as Edits::fadeIn)
    const size_t savedCount = std::min(fadeFrames, frameCount) * channelCount;
    savedSamples.assign(samples.begin(), samples.begin() + savedCount);

    Edits::fadeIn(buffer, fadeFrames);
}

void FadeInCommand::undo(AudioBuffer& buffer) {
    // Put the original samples back at the start of the buffer
    std::vector<float>& samples = buffer.getSamples();
    std::copy(savedSamples.begin(), savedSamples.end(), samples.begin());
}
