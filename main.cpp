#include <iostream>

#include "Oscillator.h"

int main() {
    auto osc = Oscillator(0.5f, 440.0f, 44100.0f);

    for (int i = 0; i < 100; i++) {
        std::cout << osc.getNextSample() << std::endl;
    }

    return 0;
}