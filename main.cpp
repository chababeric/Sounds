#include <iostream>
#include <cstdint>
#include <string>
#include <optional>

#include "Oscillator.h"
#include "AudioPlayer.h"
#include "Waveform.h"

// Turns a name typed by the user into a Waveform.
// Returns std::nullopt if the name doesn't match any waveform.
std::optional<Waveform> parseWaveform(const std::string& name) {
    if (name == "sine") {
        return Waveform::Sine;
    }
    if (name == "square") {
        return Waveform::Square;
    }
    if (name == "saw") {
        return Waveform::Saw;
    }
    if (name == "triangle") {
        return Waveform::Triangle;
    }
    return std::nullopt; // No match
}

// Turns a Waveform back into a name, for display.
std::string waveformName(Waveform waveform) {
    switch (waveform) {
        case Waveform::Sine:     return "sine";
        case Waveform::Square:   return "square";
        case Waveform::Saw:      return "saw";
        case Waveform::Triangle: return "triangle";
    }
    return "unknown";
}

// Throws away everything left on the current input line,
// so a bad command doesn't leak into the next one.
void discardRestOfLine() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Reads a number for a command. Returns std::nullopt if the user typed something else.
std::optional<float> readNumber() {
    float value = 0.0f;
    if (std::cin >> value) {
        return value;
    }

    // The read failed (e.g. "f hello"): reset the error state and skip the bad input
    std::cin.clear();
    discardRestOfLine();
    return std::nullopt;
}

void printHelp() {
    std::cout << "\nCommands:\n"
              << "  f <hz>      Set frequency (20 to 22050), e.g. f 880\n"
              << "  a <0-1>     Set amplitude (0.0 to 1.0), e.g. a 0.3\n"
              << "  w <name>    Set waveform: sine, square, saw, triangle\n"
              << "  s           Show current settings\n"
              << "  h           Show this help\n"
              << "  q           Quit\n\n";
}

void printStatus(Oscillator& oscillator) {
    std::cout << "Now playing: " << waveformName(oscillator.getWaveform())
              << ", " << oscillator.getFrequency() << " Hz"
              << ", amplitude " << oscillator.getAmplitude() << '\n';
}

int main() {
    constexpr uint32_t SAMPLE_RATE = 44100;
    constexpr uint16_t CHANNEL_COUNT = 2;       // Stereo: same tone in both speakers
    constexpr float FREQUENCY = 440.0f;         // The note A
    constexpr float AMPLITUDE = 0.2f;           // Quieter than the file: speakers can be loud

    try {
        Oscillator oscillator(AMPLITUDE, FREQUENCY, SAMPLE_RATE, Waveform::Triangle);
        AudioPlayer player(oscillator, SAMPLE_RATE, CHANNEL_COUNT);
        player.start();

        std::cout << "=== Tone Generator ===\n";
        printStatus(oscillator);
        printHelp();

        std::string command;
        while (std::cout << "> " && std::cin >> command) {
            if (command == "q") {
                break;
            }

            if (command == "f") {
                const auto frequency = readNumber();
                if (!frequency) {
                    std::cout << "Please type a number, e.g. f 880\n";
                    continue;
                }
                oscillator.setFrequency(*frequency);
                printStatus(oscillator);
            }
            else if (command == "a") {
                const auto amplitude = readNumber();
                if (!amplitude) {
                    std::cout << "Please type a number, e.g. a 0.3\n";
                    continue;
                }
                oscillator.setAmplitude(*amplitude);
                printStatus(oscillator);
            }
            else if (command == "w") {
                std::string name;
                std::cin >> name;
                const auto waveform = parseWaveform(name);
                if (!waveform) {
                    std::cout << "Unknown waveform '" << name << "'. Use: sine, square, saw, triangle\n";
                    continue;
                }
                oscillator.setWaveform(*waveform);
                printStatus(oscillator);
            }
            else if (command == "s") {
                printStatus(oscillator);
            }
            else if (command == "h") {
                printHelp();
            }
            else {
                std::cout << "Unknown command '" << command << "'. Type h for help.\n";
                discardRestOfLine();
            }
        }

        std::cout << "Bye!\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}