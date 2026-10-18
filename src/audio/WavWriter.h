//
// Created by Eric El-Chabab on 2026-10-02.
//

#ifndef SOUNDS_WAVWRITER_H
#define SOUNDS_WAVWRITER_H

#include <fstream>
#include <cstdint>
#include <string>
#include "audio/AudioBuffer.h"


class WavWriter {
public:
    static bool writeWav(const AudioBuffer& buffer, const std::string& fileName);

private:
    static void writeHeader(std::ofstream& file, const AudioBuffer& buffer);
    static int16_t floatToPcm16(float value);
};


#endif //SOUNDS_WAVWRITER_H
