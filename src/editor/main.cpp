//
// Created by Eric El-Chabab on 2026-10-30.
//
// Sound Editor: loads a WAV file, shows its waveform, applies edits and saves the result.
// Built with Dear ImGui (immediate-mode UI) on top of GLFW (window, input) and OpenGL (drawing).
//

#include <GLFW/glfw3.h>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "FadeInCommand.h"
#include "audio/AudioBuffer.h"
#include "audio/WavReader.h"
#include "audio/WavWriter.h"
#include "editor/EditCommand.h"
#include "editor/NormalizeCommand.h"
#include "editor/ReverseCommand.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

int main() {
    // 1. Open a window with an OpenGL 3.2 context (the version macOS supports)
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Sound Editor", nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);    // wait for the screen refresh: ~60 frames per second

    // 2. Set up ImGui and connect it to GLFW (input) and OpenGL (drawing)
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");

    // Editor state. Declared outside the frame loop so it survives from one frame to the next:
    // ImGui only draws the UI, the program owns the data.
    std::optional<AudioBuffer> buffer;               // the loaded audio; empty until a file is loaded

    // Undo history: every applied edit, oldest first. Undo takes the last one back.
    // Stores commands, not copies of the audio, so it costs almost no memory.
    std::vector<std::unique_ptr<EditCommand>> history;

    char loadName[256] = "roundtrip_test.wav";       // ImGui text fields need a plain char array
    char saveName[256] = "edited.wav";
    std::string statusMessage;                       // result of the last load/save, shown under the buttons
    bool isError = false;                            // true: show statusMessage in red, false: in green

    // Applies an edit to the buffer and records it in the history so it can be undone.
    // [&]: captures history and buffer by reference.
    auto applyEdit = [&](std::unique_ptr<EditCommand> command) {
        command->apply(*buffer);
        history.push_back(std::move(command));   // the history now owns the command
    };

    const ImVec4 RED(1.0f, 0.4f, 0.4f, 1.0f);
    const ImVec4 GREEN(0.4f, 1.0f, 0.4f, 1.0f);

    // 3. The frame loop: runs until the window is closed
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();                       // read mouse and keyboard

        ImGui_ImplOpenGL3_NewFrame();           // start a new ImGui frame
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ---------- UI: rebuilt every frame ----------
        ImGui::SetNextWindowSize(ImVec2(900, 400), ImGuiCond_FirstUseEver);   // initial size only
        ImGui::Begin("Sample");

        // Loading. Buttons only change data; everything below displays it.
        ImGui::InputText("Load file", loadName, sizeof(loadName));
        ImGui::SameLine();
        if (ImGui::Button("Load")) {
            history.clear();
            buffer = WavReader::readWav(loadName);
            isError = !buffer;
            statusMessage = buffer ? std::string("Loaded ") + loadName
                                   : std::string("Could not load ") + loadName
                                     + " (missing, or not a 16-bit PCM WAV)";
        }

        // Everything below needs a loaded buffer
        if (buffer) {
            // Saving
            ImGui::InputText("Save as", saveName, sizeof(saveName));
            ImGui::SameLine();
            if (ImGui::Button("Save")) {
                const bool saved = WavWriter::writeWav(*buffer, saveName);
                isError = !saved;
                statusMessage = saved ? std::string("Saved ") + saveName
                                      : std::string("Could not save ") + saveName;
            }

            // File info
            const size_t frameCount = buffer->getSamples().size() / buffer->getChannelCount();
            ImGui::Text("Sample rate: %u Hz", static_cast<unsigned>(buffer->getSampleRate()));
            ImGui::Text("Channels: %u", static_cast<unsigned>(buffer->getChannelCount()));
            ImGui::Text("Duration: %.2f s", buffer->getDuration());
            ImGui::Text("Frames: %zu", frameCount);

            // Edits: applied through applyEdit so they can be undone;
            // the plot below shows the result on the next frame
            if (ImGui::Button("Reverse")) {
                applyEdit(std::make_unique<ReverseCommand>());
            }
            ImGui::SameLine();
            if (ImGui::Button("Normalize")) {
                applyEdit(std::make_unique<NormalizeCommand>(0.9f));
            }
            ImGui::SameLine();
            if (ImGui::Button("Fade In")) {
                constexpr float FADE_SECONDS = 0.5f;
                const auto fadeFrames = static_cast<size_t>(FADE_SECONDS * static_cast<float>(buffer->getSampleRate()));
                applyEdit(std::make_unique<FadeInCommand>(fadeFrames));
            }
            ImGui::SameLine();

            // Undo: revert the last edit, then delete it. Disabled (grayed out) when there is none.
            ImGui::BeginDisabled(history.empty());
            if (ImGui::Button("Undo")) {
                history.back()->undo(*buffer);
                history.pop_back();             // unique_ptr deletes the command
            }
            ImGui::EndDisabled();

            // Waveform of the first channel: the stride skips the other channels of each frame
            ImGui::PlotLines("##waveform",                   // "##": hidden label, still a unique ID
                             buffer->getSamples().data(),
                             static_cast<int>(frameCount),
                             0, nullptr,
                             -1.0f, 1.0f,                    // fixed scale: quiet sounds look quiet
                             ImVec2(-1.0f, 200.0f),          // full width, 200 px high
                             static_cast<int>(sizeof(float) * buffer->getChannelCount()));
        }

        // Status of the last load/save, shown whether or not a buffer is loaded
        if (!statusMessage.empty()) {
            ImGui::TextColored(isError ? RED : GREEN, "%s", statusMessage.c_str());
        }

        ImGui::End();
        // ---------- end of UI ----------

        ImGui::Render();                        // turn the UI into triangles
        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);   // background color
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());   // draw them

        glfwSwapBuffers(window);                // show the finished frame
    }

    // 4. Clean up, in reverse order of setup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}