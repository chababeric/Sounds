//
// Created by Eric El-Chabab on 2026-11-13.
//

#include "editor/NormalizeCommand.h"
#include "editor/Edits.h"

NormalizeCommand::NormalizeCommand(float targetPeak)
    : targetPeak(targetPeak) {}

void NormalizeCommand::apply(AudioBuffer &buffer) {
    gain = Edits::normalize(buffer, targetPeak);
}

void NormalizeCommand::undo(AudioBuffer& buffer) {
    Edits::applyGain(buffer, 1.0f / gain);    // dividing by the gain reverts the multiplication
}
