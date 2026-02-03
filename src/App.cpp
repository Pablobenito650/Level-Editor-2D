#include "App.h"

#include <iostream>

namespace minieditor
{
        void App::Run()
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

            /*
                Initialisation GUI
                Creatiion d'IO
            */ 
            mGui.GUISet(mWindow, mRenderer);
            ImGuiIO& io = ImGui::GetIO(); (void)io;

            /* 
                Initialisation des tiles depuis le dossier Assets
                - WriteTileSet() Ecriture du fichier de mapping id-texture (TileSet.txt) 
                - InitTiles()    Chargement des textures a utiliser et stockage comme assets dans le tableau mLevel.mTileSet
                - InitLight()    Chargement de texture de lumiere pour le mini systeme de lumiere
            */
            mLevel.WriteTileSet();
            mLevel.InitTiles(mRenderer);
            mLevel.InitLight(mRenderer);

            // Pour le deltatime
            auto lastTime = std::chrono::high_resolution_clock::now();

            // Boucle principale
            while(mIsRunning)
            {
                // Pour le deltatime
                auto currentTime = std::chrono::high_resolution_clock::now();
                float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
                lastTime = currentTime;

                // Gestion des evenements
                SDL_Event event;
                while(SDL_PollEvent(&event))
                {
                    // Gestion des evenements
                    EventManage(event, io);
                }

                // Gestion Level
                mLevel.Update(deltaTime);

                SDL_RenderClear(mRenderer);

                // Rendu Level
                mLevel.Render(mRenderer);

                // Dessiner les limites du niveau
                mLevel.RenderBackGroundGrid(mRenderer);

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

        // Evenement de fermeture de l'application
        if(event.type == SDL_EVENT_QUIT)
            mIsRunning = false;
        if(event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(mWindow))
            mIsRunning = false;
        
        // Evenements utiles pour la gestion du niveau
        if(!io.WantCaptureMouse)
        {
            mLevel.MouseGetter(event);
        }

        SDL_Keycode key = event.key.key;
        SDL_Keymod mod = event.key.mod;

        if(!io.WantCaptureKeyboard)
        {
            if(event.type == SDL_EVENT_KEY_DOWN && event.key.repeat == 0)
            {   
                // Reinitialiser les parametres camera principale (position et zoom)
                if(event.key.scancode == SDL_SCANCODE_R)
                {
                    mLevel.ResetCamera();
                }

                // Mise a jour du TileSet si une texture est ajouter dans Assets pendant que le programme tourne
                if(event.key.scancode == SDL_SCANCODE_T)
                {
                    mLevel.WriteTileSet();
                    mLevel.InitTiles(mRenderer);
                }

                // Racourcis clavier pour Load/Save
                if(key == SDLK_S)
                {
                    if(mod & SDL_KMOD_CTRL)
                    {
                        // Sauvegarde du niveau
                        mLevel.SaveLevel();
                    }
                }

                if(key == SDLK_O)
                {
                    if(mod & SDL_KMOD_CTRL)
                    {
                        // Chargement du niveau
                        mLevel.LoadLevel();
                    }
                }
            }
        }
    }

    void App::Close()
    {
        std::cout << "\nFermeture de l'Application" << std::endl;

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

