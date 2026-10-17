#include <cstdint>
#include <iostream>
#include <limits>
#include <optional>
#include <string>

#include "audio/AudioPlayer.h"
#include "synth/Synth.h"
#include "synth/Waveform.h"

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

// Reads a whole number for a command. Returns std::nullopt if the user typed something else.
std::optional<int> readInt() {
    int value = 0;
    if (std::cin >> value) {
        return value;
    }

    // The read failed (e.g. "on hello"): reset the error state and skip the bad input
    std::cin.clear();
    discardRestOfLine();
    return std::nullopt;
}

void printHelp() {
    std::cout << "\nCommands:\n"
              << "  on <0-127>  Start a note, e.g. on 60 (middle C)\n"
              << "  off <0-127> Release a note, e.g. off 60\n"
              << "  w <name>    Set waveform for all voices: sine, square, saw, triangle\n"
              << "  s           Show current settings\n"
              << "  h           Show this help\n"
              << "  q           Quit\n\n"
              << "Try a chord: on 60, on 64, on 67\n\n";
}

void printStatus(const Synth& synth) {
    std::cout << "Waveform: " << waveformName(synth.getWaveform())
              << ", voices playing: " << synth.getActiveVoiceCount()
              << " of " << Synth::MAX_VOICES << '\n';
}

int main() {
    constexpr uint32_t SAMPLE_RATE = 44100;
    constexpr uint16_t CHANNEL_COUNT = 2;       // Stereo: same sound in both speakers

    try {
        Synth synth(SAMPLE_RATE);
        AudioPlayer player(synth, SAMPLE_RATE, CHANNEL_COUNT);
        player.start();

        std::cout << "=== Tiny Synth ===\n";
        printStatus(synth);
        printHelp();

        std::string command;
        while (std::cout << "> " && std::cin >> command) {
            if (command == "q") {
                break;
            }

            if (command == "on") {
                const auto note = readInt();
                if (!note) {
                    std::cout << "Please type a note number, e.g. on 60\n";
                    continue;
                }
                synth.noteOn(*note);
                printStatus(synth);
            }
            else if (command == "off") {
                const auto note = readInt();
                if (!note) {
                    std::cout << "Please type a note number, e.g. off 60\n";
                    continue;
                }
                synth.noteOff(*note);
                printStatus(synth);
            }
            else if (command == "w") {
                std::string name;
                std::cin >> name;
                const auto waveform = parseWaveform(name);
                if (!waveform) {
                    std::cout << "Unknown waveform '" << name << "'. Use: sine, square, saw, triangle\n";
                    continue;
                }
                synth.setWaveform(*waveform);
                printStatus(synth);
            }
            else if (command == "s") {
                printStatus(synth);
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