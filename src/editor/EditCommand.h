//
// Created by Eric El-Chabab on 2026-11-06.
//

#ifndef SOUNDS_EDITCOMMAND_H
#define SOUNDS_EDITCOMMAND_H

#include "audio/AudioBuffer.h"

// Base class of every undoable edit (Command pattern).
// Instead of saving a copy of the audio, the history stores these objects:
// each one knows how to apply its change and how to undo it.
class EditCommand {
public:
    // Virtual: the history deletes commands through EditCommand pointers (unique_ptr).
    virtual ~EditCommand() = default;

    // Applies the edit to the buffer.
    virtual void apply(AudioBuffer& buffer) = 0;

    // Reverts the edit. Must be called on the buffer as apply() left it.
    virtual void undo(AudioBuffer& buffer) = 0;
};

#endif //SOUNDS_EDITCOMMAND_H
