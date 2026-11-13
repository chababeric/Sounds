//
// Created by Eric El-Chabab on 2026-11-13.
//

#ifndef SOUNDS_FADEINCOMMAND_H
#define SOUNDS_FADEINCOMMAND_H

#include "editor/EditCommand.h"
#include <cstddef>
#include <vector>

// Fades the start of the audio in from silence.
// Can't be undone with math (frame 0 is multiplied by 0, which loses the original),
// so it saves the original samples of the faded part only: a partial snapshot.
class FadeInCommand : public EditCommand {
public:
    // fadeFrames: length of the fade in frames (seconds × sample rate).
    explicit FadeInCommand(size_t fadeFrames);

    void apply(AudioBuffer& buffer) override;
    void undo(AudioBuffer& buffer) override;

private:
    // Requested length of the fade. Clamped to the file length when applied.
    size_t fadeFrames;

    // Original samples of the faded part, saved by apply() and restored by undo().
    std::vector<float> savedSamples;
};

#endif //SOUNDS_FADEINCOMMAND_H
