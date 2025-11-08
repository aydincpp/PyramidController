#ifndef GUI_H
#define GUI_H

#include <string>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

struct Gui {
    bool init(GLFWwindow* window, const std::string& glslVersion)
    {
        IMGUI_CHECKVERSION();
        if (!ImGui::CreateContext()) {
            fprintf(stderr, "err: failed to create imgui context\n");
            return false;
        }

        if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
            fprintf(stderr, "err: failed to initialize ImGui_ImplGlfw_InitForOpenGL\n");
            return false;
        }

        if (!ImGui_ImplOpenGL3_Init(glslVersion.c_str())) {
            fprintf(stderr, "err: failed to initialize ImGui_ImplOpenGL3_Init\n");
            return false;
        }

        return true;
    }

    void newFrame()
    {
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();
    }

    void render()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void shutdown()
    {
        ImGui_ImplGlfw_Shutdown();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui::DestroyContext();
    }
};

#endif // GUI_H
