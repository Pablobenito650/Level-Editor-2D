#pragma once
#include "GUI.h"

namespace minieditor
{
    /**
     * @brief Creation et gestion de l'Application
     */
    class App
    {
        private:
            // Outils SDL
            SDL_Window* mWindow;
            SDL_Renderer* mRenderer;

            // Caracteristiques de la fenetre
            const int mWidth = 1920;
            const int mHeight = 1056;

            // Arret de l'app
            bool mIsRunning = true;
        
        public:
            GUI mGui;
            Level mLevel;

            void Run();
            void Close();

            /**
             * @brief gestion des evenements clavier et souris
             * 
             * @param event Event SDL
             * @param io ImGui IO
             */
            void EventManage(const SDL_Event& event, ImGuiIO io);
    };
}