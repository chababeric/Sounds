#include <cstdint>
#include <iostream>
#include <limits>
#include <optional>
#include <string>
#include <memory>
#include <cmath>

#include "audio/AudioPlayer.h"
#include "synth/Synth.h"
#include "synth/Waveform.h"
#include "effects/Distortion.h"
#include "effects/Delay.h"
#include "effects/LowPassFilter.h"
#include "audio/AudioBuffer.h"
#include "audio/WavReader.h"
#include "audio/WavWriter.h"

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

// Reads a decimal number for a command. Returns std::nullopt if the user typed something else.
std::optional<float> readFloat() {
    float value = 0.0f;
    if (std::cin >> value) {
        return value;
    }

    // The read failed (e.g. "d hello"): reset the error state and skip the bad input
    std::cin.clear();
    discardRestOfLine();
    return std::nullopt;
}

void printHelp() {
    std::cout << "\nCommands:\n"
              << "  on <0-127>      Start a note, e.g. on 60 (middle C)\n"
              << "  off <0-127>     Release a note, e.g. off 60\n"
              << "  w <name>        Set waveform for all voices: sine, square, saw, triangle\n"
              << "  d <1-20>        Set distortion drive, e.g. d 10\n"
              << "  dt <seconds>    Set delay time in seconds, e.g. dt 0.4\n"
              << "  fb <0-0.95>     Set feedback, e.g. fb 0.4\n"
              << "  mix <0-1>       Set mix level, e.g. mix 0.3\n"
              << "  lp <hz>         Set filter cutoff in Hz, e.g. lp 1500\n"
              << "  dist <on/off>   Turn the distortion on or off, e.g. dist off\n"
              << "  delay <on/off>  Turn the delay on or off, e.g. delay off\n"
              << "  filter <on/off> Turn the filter on or off, e.g. filter off\n"
              << "  test            Write a WAV file, read it back and compare\n"
              << "  s               Show current settings\n"
              << "  h               Show this help\n"
              << "  q               Quit\n\n"
              << "Try a chord: on 60, on 64, on 67\n\n";
}

void printStatus(const Synth& synth, const Distortion& distortion,
                 const Delay& delay, const LowPassFilter& filter) {
    std::cout << "Waveform: " << waveformName(synth.getWaveform())
              << ", voices playing: " << synth.getActiveVoiceCount()
              << " of " << Synth::MAX_VOICES << '\n'
              << "Distortion: " << (distortion.isEnabled() ? "on" : "off")
              << ", drive " << distortion.getDrive() << '\n'
              << "Delay: " << (delay.isEnabled() ? "on" : "off")
              << ", time " << delay.getDelayTime() << " s"
              << ", feedback " << delay.getFeedback()
              << ", mix " << delay.getMix() << '\n'
              << "Filter: " << (filter.isEnabled() ? "on" : "off")
              << ", cutoff " << filter.getCutoff() << " Hz\n";
}

// Renders 1 second of A4 to a WAV file, reads it back with WavReader,
// and checks that the audio survived the trip. Returns true if it did.
bool runWavRoundTripTest(uint32_t sampleRate) {
    const std::string fileName = "roundtrip_test.wav";
    constexpr int TEST_NOTE = 69;                       // A4, 440 Hz
    constexpr float TOLERANCE = 1.0f / 32767.0f;        // one 16-bit step

    // A separate synth, not the live one: the audio thread owns that one
    Synth renderSynth(sampleRate);
    renderSynth.noteOn(TEST_NOTE);

    AudioBuffer original(sampleRate, 1);                // mono
    for (uint32_t i = 0; i < sampleRate; i++) {         // 1 second
        original.addSample(renderSynth.getNextSample());
    }

    if (!WavWriter::writeWav(original, fileName)) {
        std::cout << "Round trip FAILED: could not write " << fileName << '\n';
        return false;
    }

    const auto loaded = WavReader::readWav(fileName);
    if (!loaded) {
        std::cout << "Round trip FAILED: could not read " << fileName << '\n';
        return false;
    }

    // Header values must match exactly
    if (loaded->getSampleRate() != original.getSampleRate() ||
        loaded->getChannelCount() != original.getChannelCount() ||
        loaded->getSamples().size() != original.getSamples().size()) {
        std::cout << "Round trip FAILED: format mismatch\n"
                  << "  sample rate " << original.getSampleRate() << " -> " << loaded->getSampleRate() << '\n'
                  << "  channels    " << original.getChannelCount() << " -> " << loaded->getChannelCount() << '\n'
                  << "  samples     " << original.getSamples().size() << " -> " << loaded->getSamples().size() << '\n';
        return false;
    }

    // Samples only need to be close: 16-bit rounding loses a tiny bit of precision
    const auto& before = original.getSamples();
    const auto& after = loaded->getSamples();
    for (size_t i = 0; i < before.size(); i++) {
        if (std::abs(before[i] - after[i]) > TOLERANCE) {
            std::cout << "Round trip FAILED at sample " << i
                      << ": " << before[i] << " -> " << after[i] << '\n';
            return false;
        }
    }

    std::cout << "Round trip OK: " << after.size() << " samples, "
              << loaded->getDuration() << " s, written to " << fileName << '\n';
    return true;
}

int main() {
    constexpr uint32_t SAMPLE_RATE = 44100;
    constexpr uint16_t CHANNEL_COUNT = 2;       // Stereo: same sound in both speakers

    try {
        Synth synth(SAMPLE_RATE);
        AudioPlayer player(synth, SAMPLE_RATE, CHANNEL_COUNT);

        // Create the distortion, delay and low pass filter; main owns them for now
        auto distortion = std::make_unique<Distortion>(1.0f);
        auto delay = std::make_unique<Delay>(SAMPLE_RATE, 2.0f);
        auto lowPassFilter = std::make_unique<LowPassFilter>(SAMPLE_RATE, 20000.0f);

        // Keep a plain pointer to it, to control it later (observing, not owning)
        Distortion* distortionControl = distortion.get();
        Delay* delayControl = delay.get();
        LowPassFilter* lowPassFilterControl = lowPassFilter.get();

        synth.addEffect(std::move(distortion));
        synth.addEffect(std::move(delay));
        synth.addEffect(std::move(lowPassFilter));

        distortionControl->setEnabled(false);
        delayControl->setEnabled(false);
        lowPassFilterControl->setEnabled(false);

        player.start();

        std::cout << "=== Tiny Synth ===\n";
        printStatus(synth, *distortionControl, *delayControl, *lowPassFilterControl);
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
                printStatus(synth, *distortionControl, *delayControl, *lowPassFilterControl);
            }
            else if (command == "off") {
                const auto note = readInt();
                if (!note) {
                    std::cout << "Please type a note number, e.g. off 60\n";
                    continue;
                }
                synth.noteOff(*note);
                printStatus(synth, *distortionControl, *delayControl, *lowPassFilterControl);
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
                printStatus(synth, *distortionControl, *delayControl, *lowPassFilterControl);
            }
            else if (command == "d") {
                const auto drive = readFloat();
                if (!drive) {
                    std::cout << "Please type a number from 1 to 20, e.g. d 10\n";
                    continue;
                }
                distortionControl->setDrive(*drive);
                printStatus(synth, *distortionControl, *delayControl, *lowPassFilterControl);
            }
            else if (command == "dt") {
                const auto delayTime = readFloat();
                if (!delayTime) {
                    std::cout << "Please type a time in seconds, from 0 to 2, e.g. dt 0.4\n";
                    continue;
                }
                delayControl->setDelayTime(*delayTime);
                printStatus(synth, *distortionControl, *delayControl, *lowPassFilterControl);
            }
            else if (command == "fb") {
                const auto feedback = readFloat();
                if (!feedback) {
                    std::cout << "Please type a number from 0 to 0.95, e.g. fb 0.4\n";
                    continue;
                }
                delayControl->setFeedback(*feedback);
                printStatus(synth, *distortionControl, *delayControl, *lowPassFilterControl);
            }
            else if (command == "mix") {
                const auto mixLevel = readFloat();
                if (!mixLevel) {
                    std::cout << "Please type a number from 0 to 1, e.g. mix 0.3\n";
                    continue;
                }
                delayControl->setMix(*mixLevel);
                printStatus(synth, *distortionControl, *delayControl, *lowPassFilterControl);
            }
            else if (command == "lp") {
                const auto lowPassFilterFrequency = readFloat();
                if (!lowPassFilterFrequency) {
                    std::cout << "Please type a frequency in Hz, from 20 to 22050, e.g. lp 1500\n";
                    continue;
                }
                lowPassFilterControl->setCutoff(*lowPassFilterFrequency);
                printStatus(synth, *distortionControl, *delayControl, *lowPassFilterControl);
            }
            else if (command == "dist") {
                std::string state;
                std::cin >> state;
                if (state == "on") {
                    distortionControl->setEnabled(true);
                }
                else if (state == "off") {
                    distortionControl->setEnabled(false);
                }
                else {
                    std::cout << "Please type on or off, e.g. dist on\n";
                    continue;
                }
                printStatus(synth, *distortionControl, *delayControl, *lowPassFilterControl);
            }
            else if (command == "delay") {
                std::string state;
                std::cin >> state;
                if (state == "on") {
                    delayControl->setEnabled(true);
                }
                else if (state == "off") {
                    delayControl->setEnabled(false);
                }
                else {
                    std::cout << "Please type on or off, e.g. delay on\n";
                    continue;
                }
                printStatus(synth, *distortionControl, *delayControl, *lowPassFilterControl);
            }
            else if (command == "filter") {
                std::string state;
                std::cin >> state;
                if (state == "on") {
                    lowPassFilterControl->setEnabled(true);
                }
                else if (state == "off") {
                    lowPassFilterControl->setEnabled(false);
                }
                else {
                    std::cout << "Please type on or off, e.g. filter on\n";
                    continue;
                }
                printStatus(synth, *distortionControl, *delayControl, *lowPassFilterControl);
            }
            else if (command == "test") {
                runWavRoundTripTest(SAMPLE_RATE);
            }
            else if (command == "s") {
                printStatus(synth, *distortionControl, *delayControl, *lowPassFilterControl);
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