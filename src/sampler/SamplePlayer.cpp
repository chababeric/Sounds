//
// Created by Eric El-Chabab on 2026-10-18.
//

#include <utility>
#include <algorithm>
#include "sampler/SamplePlayer.h"

SamplePlayer::SamplePlayer(AudioBuffer sample)
    : sample(std::move(sample)),
      frameCount(this->sample.getSamples().size() / this->sample.getChannelCount()) {}

float SamplePlayer::getNextSample() {
    if (triggerRequested.exchange(false)) {
        position = 0;
        playing.store(true);
    }

    if (!playing.load()) return 0.0f;

    const auto frame = static_cast<size_t>(position);

    if (frame >= frameCount) {
        playing.store(false);
        return 0.0f;
    }

    const uint16_t channelCount = sample.getChannelCount();

    float sum = 0.0f;
    for (uint16_t c = 0; c < channelCount; c++) {
        const size_t index = frame * channelCount + c;
        sum += sample.getSamples()[index];
    }

    const float result = sum / static_cast<float>(channelCount);

    position += speed.load();

    return result;
}

void SamplePlayer::trigger() {
    triggerRequested.store(true);
}

bool SamplePlayer::isPlaying() const {
    return playing.load();
}

void SamplePlayer::setSpeed(float speed) {
    this->speed.store(std::clamp(speed, 0.25f, 4.0f));
}
