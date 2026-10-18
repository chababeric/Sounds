//
// Created by Eric El-Chabab on 2026-10-18.
//

#ifndef SOUNDS_WAVREADER_H
#define SOUNDS_WAVREADER_H

#include <optional>
#include <string>
#include "audio/AudioBuffer.h"

/**
 * Loads a WAV file from disk into an AudioBuffer.
 *
 * Counterpart of WavWriter. Supports only the format WavWriter produces:
 * uncompressed PCM, 16-bit, any channel count and sample rate.
 * Samples are converted from int16 to float in the range -1..1.
 */
class WavReader {
public:
    /**
     * Reads a WAV file and returns its audio.
     *
     * @param fileName Path of the .wav file to read.
     * @return The audio, or std::nullopt if the file can't be opened,
     *         isn't a valid WAV, isn't 16-bit PCM, or is truncated.
     */
    static std::optional<AudioBuffer> readWav(const std::string& fileName);
};


#endif //SOUNDS_WAVREADER_H
