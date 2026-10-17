//
// Created by Eric El-Chabab on 2026-10-17.
//

#ifndef SOUNDS_EFFECTCHAIN_H
#define SOUNDS_EFFECTCHAIN_H

#include <vector>
#include <memory>
#include "Effect.h"

// A series of effects applied one after another, like an assembly line:
// the output of each effect becomes the input of the next.
// A chain is itself an Effect (the Composite pattern), so it can be used
// anywhere a single effect can, even inside another chain.
// An empty chain passes the sound through unchanged.
class EffectChain : public Effect {
public:
    // Passes the sample through every effect, in the order they were added.
    // Runs on the audio thread.
    float process(float sample) override;

    // Appends an effect to the end of the chain, which takes ownership of it.
    // Setup only: call before playback starts. Adding may reallocate the list,
    // which would break process() if the audio thread were looping over it.
    void add(std::unique_ptr<Effect> effect);

private:
    // The effects, in processing order. unique_ptr because each effect is a
    // different class (and size), and the chain owns them.
    std::vector<std::unique_ptr<Effect>> effects;
};

#endif //SOUNDS_EFFECTCHAIN_H
