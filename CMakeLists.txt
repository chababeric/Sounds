cmake_minimum_required(VERSION 4.3)
project(Sounds)

set(CMAKE_CXX_STANDARD 20)

add_executable(Sounds main.cpp
        Oscillator.cpp
        Oscillator.h
        AudioBuffer.cpp
        AudioBuffer.h
        WavWriter.cpp
        WavWriter.h)
