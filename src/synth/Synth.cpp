//
// Created by Eric El-Chabab on 2026-10-15.
//

#include "synth/Synth.h"
#include "synth/Voice.h"

Synth::Synth(uint32_t sampleRate) {
    voices.reserve(MAX_VOICES);

    for (int i = 0; i < MAX_VOICES; i++) {
        voices.push_back(std::make_unique<Voice>(sampleRate));
    }
}

Voice* Synth::findOldestVoice(bool released) {
    Voice* oldest = nullptr;

    for (auto& voice : voices) {
        const bool isReleased = voice->getNote() == Voice::NO_NOTE;

        if (voice->isActive() && isReleased == released) {
            if (oldest == nullptr || voice->getStartOrder() < oldest->getStartOrder()) {
                oldest = voice.get();
            }
        }
    }

    return oldest;
}

void Synth::noteOn(int note) {
    for (auto& voice : voices) {
        if (!voice->isActive()) {
            voice->noteOn(note, ++noteCounter);
            return;
        }
    }

    Voice* oldestReleased = findOldestVoice(true);

    if (oldestReleased != nullptr) {
        oldestReleased->noteOn(note, ++noteCounter);
    }
    else {
        Voice* oldestHeld = findOldestVoice(false);
        oldestHeld->noteOn(note, ++noteCounter);
    }
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
    // 1. Mix all the voices
    float sample = 0.0f;
    for (auto& voice : voices) {
        sample += voice->getNextSample();
    }

    // 2. Pass the mix through the effects, once
    sample = effectChain.process(sample);

    // 3. Apply the master volume
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

void Synth::addEffect(std::unique_ptr<Effect> effect) {
    effectChain.add(std::move(effect));
}
