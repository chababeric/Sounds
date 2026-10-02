//
// Created by Eric El-Chabab on 2026-10-02.
//

#include "WavWriter.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <limits>
#include <cmath>
#include <cstdint>

bool WavWriter::writeWav(const AudioBuffer& buffer, const std::string& fileName) {
    std::ofstream file(fileName, std::ios::binary);

    // Check if the file is open
    if (!file) {
        return false;
    }

    // Write the header of the WAV file
    writeHeader(file, buffer);

    for (float sample : buffer.getSamples()) {
        writeInt16(file, floatToPcm16(sample));
    }

    // Check the stream
    if (!file) {
        return false;
    }

    // Close first so any buffered data is written to disk, then check for errors
    file.close();
    return !file.fail();
}

void WavWriter::writeInt16(std::ofstream &file, int16_t value) {
    const auto bytes = reinterpret_cast<const char*>(&value);
    file.write(bytes, sizeof(value));
}

void WavWriter::writeUInt16(std::ofstream &file, uint16_t value) {
    const auto bytes = reinterpret_cast<const char*>(&value);
    file.write(bytes, sizeof(value));
}

void WavWriter::writeUInt32(std::ofstream& file, uint32_t value) {
    const auto bytes = reinterpret_cast<const char*>(&value);
    file.write(bytes, sizeof(value));
}

int16_t WavWriter::floatToPcm16(float value) {
    constexpr auto HIGHEST_VALUE = std::numeric_limits<int16_t>::max();

    const float clampValue = std::clamp(value, -1.0f, 1.0f);
    const long roundedValue = std::lround(clampValue * HIGHEST_VALUE);

    return static_cast<int16_t>(roundedValue);
}

// Writes the 44-byte header of a 16-bit PCM WAV file.
//
// WAV header layout (all numbers are little-endian):
//
// +--------+------+-------------------+------------------------------------------------+
// | Offset | Size | Field             | Meaning                                        |
// +--------+------+-------------------+------------------------------------------------+
// |      0 |    4 | "RIFF"            | Marks the file as a RIFF container             |
// |      4 |    4 | RIFF chunk size   | Size of the rest of the file (total - 8 bytes) |
// |      8 |    4 | "WAVE"            | Says this RIFF file contains audio             |
// +--------+------+-------------------+------------------------------------------------+
// |     12 |    4 | "fmt "            | Start of the format section (note the space)   |
// |     16 |    4 | Format chunk size | Size of the format section: 16 bytes for PCM   |
// |     20 |    2 | Audio format      | Encoding type: 1 = PCM (uncompressed)          |
// |     22 |    2 | Channels          | 1 = mono, 2 = stereo                           |
// |     24 |    4 | Sample rate       | Frames per second (e.g. 44,100)                |
// |     28 |    4 | Byte rate         | Bytes of audio per second                      |
// |     32 |    2 | Block align       | Bytes per frame (all channels)                 |
// |     34 |    2 | Bits per sample   | Bit depth: precision of each sample            |
// +--------+------+-------------------+------------------------------------------------+
// |     36 |    4 | "data"            | Start of the data section                      |
// |     40 |    4 | Data size         | Number of bytes of samples that follow         |
// |     44 |  ... | Samples           | The audio itself                               |
// +--------+------+-------------------+------------------------------------------------+
void WavWriter::writeHeader(std::ofstream& file, const AudioBuffer& buffer) {
    // --- Format constants (fixed for 16-bit PCM) ---
    constexpr uint16_t BYTES_PER_SAMPLE = 2;                    // 16-bit = 2 bytes per sample
    constexpr uint16_t BITS_PER_SAMPLE = BYTES_PER_SAMPLE * 8;  // 1 byte = 8 bits
    constexpr uint32_t FORMAT_CHUNK_SIZE = 16;                  // Bytes in the format section (offsets 20 to 35)
    constexpr uint16_t AUDIO_FORMAT_PCM = 1;                    // Code for uncompressed audio

    // --- Values calculated from the buffer ---
    // Total bytes of sample data: number of samples x bytes per sample
    const uint32_t dataSize = static_cast<uint32_t>(buffer.getSamples().size()) * BYTES_PER_SAMPLE;
    // Bytes per frame: one sample for each channel
    const auto blockAlign = static_cast<uint16_t>(buffer.getChannelCount() * BYTES_PER_SAMPLE);
    // Bytes per second: frames per second x bytes per frame
    const uint32_t byteRate = buffer.getSampleRate() * blockAlign;

    // --- RIFF section ---
    file.write("RIFF", 4);                         // File type marker
    writeUInt32(file, 36 + dataSize);              // Rest of file: 44-byte header - 8 (this tag + this field) + data
    file.write("WAVE", 4);                         // RIFF contents are audio

    // --- Format section ---
    file.write("fmt ", 4);                         // Start of format section
    writeUInt32(file, FORMAT_CHUNK_SIZE);          // Format section is 16 bytes
    writeUInt16(file, AUDIO_FORMAT_PCM);           // Uncompressed PCM
    writeUInt16(file, buffer.getChannelCount());   // Mono or stereo
    writeUInt32(file, buffer.getSampleRate());     // Frames per second
    writeUInt32(file, byteRate);                   // Bytes per second
    writeUInt16(file, blockAlign);                 // Bytes per frame
    writeUInt16(file, BITS_PER_SAMPLE);            // Bit depth

    // --- Data section ---
    file.write("data", 4);                         // Start of data section
    writeUInt32(file, dataSize);                   // Bytes of samples that follow
}