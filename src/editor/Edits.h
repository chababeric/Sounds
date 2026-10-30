//
// Created by Eric El-Chabab on 2026-10-30.
//

#ifndef SOUNDS_EDITS_H
#define SOUNDS_EDITS_H

#include "audio/AudioBuffer.h"

// Operations that modify an AudioBuffer in place.
namespace Edits {
    // Reverses the audio in time. Works frame by frame, so stereo channels stay on their side.
    void reverse(AudioBuffer& buffer);
}

#endif //SOUNDS_EDITS_H
