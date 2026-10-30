//
// Created by Eric El-Chabab on 2026-10-30.
//

#include <GLFW/glfw3.h>
#include <optional>
#include <string>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "audio/AudioBuffer.h"
#include "audio/WavReader.h"
#include "audio/WavWriter.h"
#include "editor/Edits.h"

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

    std::optional<AudioBuffer> buffer;
    char fileName[256] = "roundtrip_test.wav";
    std::string errorMessage;

    // 3. The frame loop: runs until the window is closed
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();                       // read mouse and keyboard

        ImGui_ImplOpenGL3_NewFrame();           // start a new ImGui frame
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::SetNextWindowSize(ImVec2(900, 400), ImGuiCond_FirstUseEver);

        // My UI goes here
        ImGui::Begin("Sample");

        ImGui::InputText("File", fileName, sizeof(fileName));

        // The button only changes data...
        if (ImGui::Button("Load")) {
            buffer = WavReader::readWav(fileName);
            if (buffer) {
                errorMessage.clear();
            }
            else {
                errorMessage = std::string("Could not load ") + fileName
                             + " (missing, or not a 16-bit PCM WAV)";
            }
        }

        // ...and the display reads the data, every frame
        if (buffer) {
            const size_t frameCount = buffer->getSamples().size() / buffer->getChannelCount();

            ImGui::Text("Sample rate: %u Hz", static_cast<unsigned>(buffer->getSampleRate()));
            ImGui::Text("Channels: %u", static_cast<unsigned>(buffer->getChannelCount()));
            ImGui::Text("Duration: %.2f s", buffer->getDuration());
            ImGui::Text("Frames: %zu", frameCount);

            if (ImGui::Button("Reverse")) {
                Edits::reverse(*buffer);
            }

            ImGui::PlotLines("##waveform",
                 buffer->getSamples().data(),
                 static_cast<int>(frameCount),
                 0, nullptr, -1.0f, 1.0f,
                 ImVec2(-1.0f, 200.0f),
                 static_cast<int>(sizeof(float) * buffer->getChannelCount())
            );
        }
        else if (!errorMessage.empty()) {
            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "%s", errorMessage.c_str());
        }

        ImGui::End();

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
