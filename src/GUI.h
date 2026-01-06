#pragma once
#include <SDL3/SDL.h>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "imgui_stdlib.h"
#include <stdio.h>
#include "Core/Level.h"

namespace minieditor
{
    /**
     * @brief Gestion du GUI
     */
    class GUI
    {
        public:
            ImVec4 mclearColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
            const float mtileSize = 32.0f;
            const float mpadding = 4.0f;
            const int mcolumns = 4;

        public:
            void GUISet(SDL_Window* window, SDL_Renderer* mRenderer);
            void TestGUI(SDL_Renderer* mRenderer);
    };
}

