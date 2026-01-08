#include "App.h"

#include <iostream>

namespace minieditor
{
        void App::test()
        {
            // Initialisation de SDL
            SDL_Init(SDL_INIT_VIDEO);

            // Flags et fenetre
            SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_HIDDEN);
            mWindow = SDL_CreateWindow("Level Editor", mWidth, mHeight, windowFlags);
            if(mWindow == nullptr)
            {
                std::cout << "Erreur fenetre : %s\n", SDL_GetError();
                return;
            }

            // SDL_Renderer
            mRenderer = SDL_CreateRenderer(mWindow, nullptr);
            if(mRenderer == nullptr)
            {
                std::cout << "Erreur de renderer : %s\n", SDL_GetError();
            }

            SDL_ShowWindow(mWindow);

            // Initialisation GUI
            mGui.GUISet(mWindow, mRenderer);
            ImGuiIO& io = ImGui::GetIO(); (void)io;

            // Initialisation des tiles depuis le dossier Assets
            mLevel.WriteTileSet();
            mLevel.InitTiles(mRenderer);

            // Boucle principale
            while(mIsRunning)
            {
                // Gestion des evenements
                SDL_Event event;
                while(SDL_PollEvent(&event))
                {
                    EventManage(event, io);
                }

                // Gestion Level
                mLevel.Update();

                SDL_RenderClear(mRenderer);

                // Rendu Level
                mLevel.Render(mRenderer);

                // Gestion GUI
                mGui.TestGUI(mRenderer);

                SDL_RenderPresent(mRenderer);
            }

            // Fermeture application
            Close();
    }

    void App::EventManage(const SDL_Event& event, ImGuiIO io)
    {
        ImGui_ImplSDL3_ProcessEvent(&event);
        if(event.type == SDL_EVENT_QUIT)
            mIsRunning = false;
        if(event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(mWindow))
            mIsRunning = false;
        
        if(!io.WantCaptureMouse)
        {
            mLevel.MouseGetter(event);
        }

        if(!io.WantCaptureKeyboard)
        {
            if(event.type == SDL_EVENT_KEY_DOWN && event.key.repeat == 0)
            {
                if(event.key.scancode == SDL_SCANCODE_S)
                {
                    mLevel.SaveLevel();
                }
                
                if(event.key.scancode == SDL_SCANCODE_R)
                {
                    mLevel.ResetCamera();
                }

                if(event.key.scancode == SDL_SCANCODE_T)
                {
                    mLevel.WriteTileSet();
                    mLevel.InitTiles(mRenderer);
                }
            }
        }
    }

    void App::Close()
    {
        std::cout << "Fermeture de l'Application" << std::endl;

        // Detruire les textures du tileset
        mLevel.DestroyTextures();

        // Arret Dear ImGUI
        ImGui_ImplSDL3_Shutdown();
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui::DestroyContext();

        // Arret SDL
        SDL_DestroyRenderer(mRenderer);
        SDL_DestroyWindow(mWindow);
        SDL_Quit();
    }
}

