//
// Created by Eric El-Chabab on 2026-11-13.
//

#ifndef SOUNDS_NORMALIZECOMMAND_H
#define SOUNDS_NORMALIZECOMMAND_H

#include "editor/EditCommand.h"

// Normalizes the audio to a target peak. Undo divides by the gain that apply() used,
// so it only stores one number instead of a copy of the audio.
// (float rounding can differ in the last digit after undo: far below audible.)
class NormalizeCommand : public EditCommand {
public:
    // targetPeak: level the loudest sample should reach, e.g. 0.9 to keep some headroom.
    explicit NormalizeCommand(float targetPeak);

    void apply(AudioBuffer& buffer) override;
    void undo(AudioBuffer& buffer) override;

private:
    // Level the loudest sample should reach.
    float targetPeak;

    // Gain applied by apply(), kept for undo(). Can't be recomputed afterwards,
    // because after normalizing the peak is already at targetPeak.
    float gain{1.0f};
};


#endif //SOUNDS_NORMALIZECOMMAND_H
