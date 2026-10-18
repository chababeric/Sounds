//
// Created by Eric El-Chabab on 2026-10-18.
//

#include <cstdint>
#include <fstream>
#include <limits>
#include <string>
#include <utility>
#include <vector>
#include "audio/WavReader.h"

namespace {
    /**
     * Reads sizeof(T) raw bytes from the file and returns them as a T.
     * WAV is little-endian, like the Mac, so no byte swapping is needed.
     * On failure, returns T{} and puts the stream in a fail state.
     */
    template <typename T>
    T readValue(std::ifstream& file) {
        T value{};
        file.read(reinterpret_cast<char*>(&value), sizeof(value));
        return value;
    }

    /// Reads a 4-character chunk ID such as "RIFF", "WAVE", "fmt " or "data".
    std::string readId(std::ifstream& file) {
        char id[4]{};
        file.read(id, 4);
        return {id, 4};
    }

    /// Converts a 16-bit PCM sample to a float in -1..1 (inverse of floatToPcm16).
    float pcm16ToFloat(int16_t sample) {
        return static_cast<float>(sample) / static_cast<float>(std::numeric_limits<int16_t>::max());
    }
}

std::optional<AudioBuffer> WavReader::readWav(const std::string &fileName) {
    std::ifstream file(fileName, std::ios::binary);

    if (!file) return std::nullopt;

    // RIFF header: "RIFF" <file size - 8> "WAVE"
    if (readId(file) != "RIFF") return std::nullopt;
    readValue<uint32_t>(file);    // file size, not needed
    if (readId(file) != "WAVE") return std::nullopt;

    // fmt chunk: describes how the samples are stored
    if (readId(file) != "fmt ") return std::nullopt;

    const auto chunkSize = readValue<uint32_t>(file);
    if (chunkSize < 16) return std::nullopt;    // too small to hold the PCM fields

    const auto audioFormat = readValue<uint16_t>(file);
    if (audioFormat != 1) return std::nullopt;    // 1 = uncompressed PCM

    const auto channelCount = readValue<uint16_t>(file);
    if (channelCount == 0) return std::nullopt;

    const auto sampleRate = readValue<uint32_t>(file);
    if (sampleRate == 0) return std::nullopt;

    const auto byteRate = readValue<uint32_t>(file);

    // Consistency checks: a mismatch means a corrupt header
    const auto blockAlign = readValue<uint16_t>(file);
    if (blockAlign != channelCount * 2) return std::nullopt;    // bytes per frame
    if (byteRate != sampleRate * blockAlign) return std::nullopt;    // bytes per second

    const auto bitsPerSample = readValue<uint16_t>(file);
    if (bitsPerSample != 16) return std::nullopt;

    file.ignore(chunkSize - 16);    // skip extra fmt bytes, after the 16 bytes of fields

    if (!file) return std::nullopt;    // file ended inside the header

    // data chunk: the samples themselves
    // TODO: skip other chunks (e.g. "LIST") that some programs put before "data"
    if (readId(file) != "data") return std::nullopt;

    const auto dataSize = readValue<uint32_t>(file);
    if (dataSize % blockAlign != 0) return std::nullopt;    // partial frame

    const uint32_t sampleCount = dataSize / 2;    // 2 bytes per 16-bit sample

    std::vector<float> samples;
    samples.reserve(sampleCount);    // allocate once; size() is still 0

    for (uint32_t i = 0; i < sampleCount; i++) {
        samples.push_back(pcm16ToFloat(readValue<int16_t>(file)));
    }

    if (!file) return std::nullopt;    // file shorter than dataSize said

    return AudioBuffer(sampleRate, channelCount, std::move(samples));
}
