//
// Created by Eric El-Chabab on 2026-11-06.
//

#ifndef SOUNDS_REVERSECOMMAND_H
#define SOUNDS_REVERSECOMMAND_H

#include "editor/EditCommand.h"

// Reverses the audio in time. Its own inverse: undo reverses again,
// so it needs no stored data.
class ReverseCommand : public EditCommand {
public:
    void apply(AudioBuffer& buffer) override;
    void undo(AudioBuffer& buffer) override;
};

#endif //SOUNDS_REVERSECOMMAND_H
