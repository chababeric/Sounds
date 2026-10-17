//
// Created by Eric El-Chabab on 2026-10-02.
//

#ifndef SOUNDS_WAVWRITER_H
#define SOUNDS_WAVWRITER_H
#include "AudioBuffer.h"
#include <fstream>
#include <cstdint>
#include <string>


class WavWriter {
public:
    static bool writeWav(const AudioBuffer& buffer, const std::string& fileName);

private:
    static void writeHeader(std::ofstream& file, const AudioBuffer& buffer);
    static void writeInt16(std::ofstream& file, int16_t value);
    static void writeUInt16(std::ofstream& file, uint16_t value);
    static void writeUInt32(std::ofstream& file, uint32_t value);
    static int16_t floatToPcm16(float value);
};


#endif //SOUNDS_WAVWRITER_H
