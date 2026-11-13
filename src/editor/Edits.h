//
// Created by Eric El-Chabab on 2026-10-30.
//

#ifndef SOUNDS_EDITS_H
#define SOUNDS_EDITS_H

#include "audio/AudioBuffer.h"

// Operations that modify an AudioBuffer in place.
// Used by the edit commands; they don't record anything for Undo themselves.
namespace Edits {
    // Reverses the audio in time. Works frame by frame, so stereo channels stay on their side.
    void reverse(AudioBuffer& buffer);

    // Scales the audio so its loudest sample reaches targetPeak (1.0 = full scale).
    // Returns the gain that was applied, so it can be undone with applyGain(buffer, 1 / gain).
    // Does nothing on silent or empty buffers and returns 1 ("no change").
    float normalize(AudioBuffer& buffer, float targetPeak);

    // Multiplies every sample by gain: above 1 = louder, below 1 = quieter.
    void applyGain(AudioBuffer& buffer, float gain);
}

#endif //SOUNDS_EDITS_H
