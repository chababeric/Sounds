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
    if (stopRequested.exchange(false)) {
        playing.store(false);
    }

    if (triggerRequested.exchange(false)) {
        position = 0.0;
        playing.store(true);
    }

    if (!playing.load()) return 0.0f;

    auto frame = static_cast<size_t>(position);

    while (frame >= frameCount) {
        if (!looping.load() || frameCount == 0) {
            playing.store(false);
            return 0.0f;
        }
        // Loop: jump back by one full length, keeping the fractional part
        position -= static_cast<double>(frameCount);
        frame = static_cast<size_t>(position);
    }

    const float a = readFrame(frame);

    const bool isLastFrame = frame + 1 >= frameCount;
    const float b = !isLastFrame ? readFrame(frame + 1)
                  : looping.load() ? readFrame(0)
                  : a;

    const auto fraction = static_cast<float>(position - static_cast<double>(frame));

    position += speed.load();

    return a + (b - a) * fraction;
}

void SamplePlayer::trigger() {
    triggerRequested.store(true);
}

void SamplePlayer::stop() {
    stopRequested.store(true);
}

bool SamplePlayer::isPlaying() const {
    return playing.load();
}

void SamplePlayer::setSpeed(float speed) {
    this->speed.store(std::clamp(speed, 0.25f, 4.0f));
}

float SamplePlayer::readFrame(size_t frame) const {
    const uint16_t channelCount = sample.getChannelCount();

    float sum = 0.0f;
    for (uint16_t c = 0; c < channelCount; c++) {
        const size_t index = frame * channelCount + c;
        sum += sample.getSamples()[index];
    }

    return sum / static_cast<float>(channelCount);
}

void SamplePlayer::setLooping(bool looping) {
    this->looping.store(looping);
}
