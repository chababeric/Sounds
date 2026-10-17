//
// Created by Eric El-Chabab on 2026-10-17.
//

#include "EffectChain.h"

float EffectChain::process(float sample) {
    for (const auto& effect : effects) {
        sample = effect->process(sample);
    }

    return sample;
}

void EffectChain::add(std::unique_ptr<Effect> effect) {
    effects.push_back(std::move(effect));
}
