cmake_minimum_required(VERSION 4.3)
project(Sounds)

set(CMAKE_CXX_STANDARD 20)

# ---------- Shared audio code: used by both the synth and the editor ----------
add_library(sound_core STATIC
        src/audio/AudioBuffer.cpp
        src/audio/AudioBuffer.h
        src/audio/WavWriter.cpp
        src/audio/WavWriter.h
        src/audio/WavReader.cpp
        src/audio/WavReader.h
)
# PUBLIC: anyone who links sound_core can also #include "audio/..."
target_include_directories(sound_core PUBLIC src)

# ---------- Tiny Synth ----------
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

        src/audio/miniaudio.cpp
        src/audio/AudioPlayer.cpp
        src/audio/AudioPlayer.h

        src/effects/Effect.h
        src/effects/Distortion.cpp
        src/effects/Distortion.h
        src/effects/EffectChain.cpp
        src/effects/EffectChain.h
        src/effects/Delay.cpp
        src/effects/Delay.h
        src/effects/LowPassFilter.cpp
        src/effects/LowPassFilter.h

        src/sampler/SamplePlayer.cpp
        src/sampler/SamplePlayer.h
)

target_include_directories(Sounds PRIVATE
        src
        external/miniaudio
)
target_link_libraries(Sounds PRIVATE sound_core)

# ---------- Editor: Dear ImGui + GLFW ----------
include(FetchContent)

FetchContent_Declare(glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.91.0)

FetchContent_MakeAvailable(glfw imgui)

# ImGui has no CMake file of its own, so we list its sources ourselves
add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)

find_package(OpenGL REQUIRED)
target_link_libraries(imgui PUBLIC glfw OpenGL::GL)
target_compile_definitions(imgui PUBLIC GL_SILENCE_DEPRECATION)   # macOS marks OpenGL as deprecated

add_executable(editor
        src/editor/main.cpp
        src/editor/Edits.cpp
        src/editor/Edits.h
)
target_include_directories(editor PRIVATE src)
target_link_libraries(editor PRIVATE imgui sound_core)
