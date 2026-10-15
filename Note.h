//
// Created by Eric El-Chabab on 2026-10-15.
//

#ifndef SOUNDS_NOTE_H
#define SOUNDS_NOTE_H

// Converts a MIDI note number to its frequency in Hz.
// note: 0 to 127 (60 = middle C, 69 = A4 = 440 Hz). Values outside are clamped.
float noteToFrequency(int note);

#endif //SOUNDS_NOTE_H
