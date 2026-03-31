#pragma once

#include <android/native_window.h>
#include "imgui.h"

uintptr_t m_IL2CPP;
bool g_Initialized;
void *m_EGL;
int glWidth, glHeight;
//int glScreenWidth, glScreenHeight;
bool clearMousePos;

int screenWidth, screenHeight;

void setRes(int _screenWidth, int _screenHeight) {
    screenWidth = _screenWidth;
    screenHeight = _screenHeight;
}

void ImplAndroid_NewFrame()
{
    ImGuiIO& io = ImGui::GetIO();
    // Setup display size (every frame to accommodate for window resizing)
    io.DisplaySize = ImVec2((float)screenWidth, (float)screenHeight);
    if (glWidth > 0 && glHeight > 0)
        io.DisplayFramebufferScale = ImVec2((float)glWidth / screenWidth, (float)glHeight / screenHeight);
}

int32_t (*orig_ANativeWindow_getWidth)(ANativeWindow* window);
int32_t hook_ANativeWindow_getWidth(ANativeWindow* window) {
    const int32_t width = orig_ANativeWindow_getWidth(window);
    screenWidth = width;
    return width;
}

int32_t (*orig_ANativeWindow_getHeight)(ANativeWindow* window);
int32_t hook_ANativeWindow_getHeight(ANativeWindow* window) {
    const int32_t height = orig_ANativeWindow_getHeight(window);
    screenHeight = height;
    return height;
}
