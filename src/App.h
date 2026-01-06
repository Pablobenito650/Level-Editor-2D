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
            SDL_Window* mWindow;
            SDL_Renderer* mRenderer;

            const int mWidth = 1024;
            const int mHeight = 992;

            bool mIsRunning = true;
        
        public:
            GUI mGui;
            Level mLevel;

            void test();
            void Close();

            /**
             * @brief gestion des evenements clavier et souris
             */
            void EventManage(const SDL_Event& event, ImGuiIO io);
    };
}