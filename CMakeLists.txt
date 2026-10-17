cmake_minimum_required(VERSION 4.3)
project(Sounds)

set(CMAKE_CXX_STANDARD 20)

add_executable(Sounds
        src/main.cpp

        external/miniaudio/miniaudio.h

        src/synth/Oscillator.cpp
        src/synth/Oscillator.h
        src/synth/Waveform.h
        src/synth/Envelope.cpp
        src/synth/Envelope.h
        src/synth/Voice.cpp
        src/synth/Voice.h
        src/synth/Note.cpp
        src/synth/Note.h
        src/synth/Synth.cpp
        src/synth/Synth.h
        src/synth/WaveShape.cpp
        src/synth/WaveShape.h

        src/audio/AudioBuffer.cpp
        src/audio/AudioBuffer.h
        src/audio/WavWriter.cpp
        src/audio/WavWriter.h
        src/audio/miniaudio.cpp
        src/audio/AudioPlayer.cpp
        src/audio/AudioPlayer.h
        
        src/effects/Effect.h
)

target_include_directories(Sounds PRIVATE
        src
        external/miniaudio
)
