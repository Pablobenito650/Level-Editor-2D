#pragma once
#include <SDL3/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <imgui_stdlib.h>
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
            // BackGround
            ImVec4 mClearColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
            
            const float TILE_SIZE = 32.0f;
            const float PADDING = 4.0f;
            const int COLUMNS = 4;
            int mIndex = 0;

        public:
            /** 
             * @brief Intialisation de Dear Imgui
             * 
             * @param window Fenetre SDL 
             * @param renderer Renderer SDL
            */
            void GUISet(SDL_Window* window, SDL_Renderer* renderer);

            /**
             * @brief Gestion de l'interface ImGui et manipulation du niveau
             * 
             * @param renderer renderer SDL
             */
            void TestGUI(SDL_Renderer* renderer);

            /*
                Inpection de tile (Assets et Tile dans la scene)
            */
            void Inspector();

            /*
                Affichage de la liste des lumieres dans la scene (hierarchie)
            */
            void LightHierarchie();

            /*
                Modification des proprietes d'une lumiere
            */
            void LightingSettings();
    };
}

