#include <iostream>
#include <cstdint>

#include "Oscillator.h"
#include "AudioBuffer.h"
#include "WavWriter.h"
#include "AudioPlayer.h"

int main() {
    constexpr uint32_t SAMPLE_RATE = 44100;
    constexpr uint16_t CHANNEL_COUNT = 2;       // Stereo: same tone in both speakers
    constexpr float FREQUENCY = 440.0f;         // The note A
    constexpr float AMPLITUDE = 0.2f;           // Quieter than the file: speakers can be loud

    try {
        Oscillator oscillator(AMPLITUDE, FREQUENCY, SAMPLE_RATE);
        AudioPlayer player(oscillator, SAMPLE_RATE, CHANNEL_COUNT);

        player.start();
        std::cout << "Playing a " << FREQUENCY <<"Hz tone. Press Enter to stop...\n";
        std::cin.get();
        player.stop();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}

// int main() {
//     // --- Settings for the tone ---
//     constexpr uint32_t SAMPLE_RATE = 44100;    // CD quality
//     constexpr uint16_t CHANNEL_COUNT = 1;      // Mono
//     constexpr float DURATION_SECONDS = 2.0f;   // 2 seconds
//     constexpr float FREQUENCY = 440.0f;        // The note A
//     constexpr float AMPLITUDE = 0.5f;          // Half volume
//
//     AudioBuffer buffer(SAMPLE_RATE, CHANNEL_COUNT);
//     Oscillator oscillator(AMPLITUDE, FREQUENCY, buffer.getSampleRate());
//
//     // Number of frames = seconds x frames per second (88 200 for 2 seconds)
//     const auto frameCount = static_cast<uint32_t>(DURATION_SECONDS * SAMPLE_RATE);
//
//     // Mono: one sample per frame
//     for (uint32_t i = 0; i < frameCount; ++i) {
//         buffer.addSample(oscillator.getNextSample());
//     }
//
//     if (!WavWriter::writeWav(buffer, "tone.wav")) {
//         std::cerr << "Error: could not write tone.wav\n";
//         return 1;
//     }
//
//     std::cout << "Wrote tone.wav (" << buffer.getDuration() << " seconds)\n";
//     return 0;
// }