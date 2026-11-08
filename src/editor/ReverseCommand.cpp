//
// Created by Eric El-Chabab on 2026-11-06.
//

#include "editor/ReverseCommand.h"
#include "editor/Edits.h"

void ReverseCommand::apply(AudioBuffer& buffer) {
    Edits::reverse(buffer);
}

void ReverseCommand::undo(AudioBuffer& buffer) {
    Edits::reverse(buffer);    // reversing twice gives back the original
}
