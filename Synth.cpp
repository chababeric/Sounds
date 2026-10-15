//
// Created by Eric El-Chabab on 2026-10-15.
//

#include "Synth.h"
#include "Voice.h"

Synth::Synth(uint32_t sampleRate) {
    voices.reserve(MAX_VOICES);

    for (int i = 0; i < MAX_VOICES; i++) {
        voices.push_back(std::make_unique<Voice>(sampleRate));
    }
}

bool Synth::noteOn(int note) {
    for (auto& voice : voices) {
        if (!voice->isActive()) {
            voice->noteOn(note);
            return true;
        }
    }
    return false;   // No free voice
}

void Synth::noteOff(int note) {
    for (auto& voice : voices) {
        if (voice->getNote() == note) {
            voice->noteOff();
            break;
        }
    }
}

float Synth::getNextSample() {
    float sample = 0.0f;
    for (auto& voice : voices) {
        sample += voice->getNextSample();
    }

    return sample * masterGain.load();
}

void Synth::setWaveform(Waveform waveform) {
    for (auto& voice : voices) {
        voice->setWaveform(waveform);
    }
}

Waveform Synth::getWaveform() const {
    // All voices share the same waveform, so the first one is representative
    return voices.front()->getWaveform();
}

int Synth::getActiveVoiceCount() const {
    int count = 0;
    for (const auto& voice : voices) {
        if (voice->isActive()) {
            count++;
        }
    }
    return count;
}
