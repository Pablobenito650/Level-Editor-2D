#include "GUI.h"

namespace minieditor
{
    void GUI::GUISet(SDL_Window* window, SDL_Renderer* renderer)
    {
        // Initialisation de Dear ImGUI
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        // Type de rendu
        ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
        ImGui_ImplSDLRenderer3_Init(renderer);
    }

    void GUI::TestGUI(SDL_Renderer* renderer)
    {
        // Debut de frame ImGui
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        {
            // Menu Principale
            if(ImGui::BeginMainMenuBar())
            {
                if(ImGui::BeginMenu("File"))
                {
                    // Option pour commencer un nouveau niveau
                    if(ImGui::MenuItem("New", "None"))
                    {
                        Level::StartNewLevel();
                    }

                    // Option de sauvegarde
                    ImGui::Separator();
                    if(ImGui::MenuItem("Save", "Ctrl+S"))
                    {
                        Level::SaveLevel();
                    }

                    // Option de chargement
                    ImGui::Separator();
                    if(ImGui::MenuItem("Load", "Ctrl+O"))
                    {
                        Level::LoadLevel();
                    }

                    ImGui::EndMenu();
                }

                // Menu Gameobject
                if(ImGui::BeginMenu("GameObject"))
                {
                    // Ajouter une Local Lightning
                    if(ImGui::MenuItem("Light"))
                    {
                        Level::AddLight();
                    }

                    ImGui::EndMenu();
                }

                ImGui::EndMainMenuBar();
            }

            ImGui::SetNextWindowPos(ImVec2(0, 20));
            ImGui::SetNextWindowSize(ImVec2(250, 540));

            // Creation de la fenetre principale
            ImGui::Begin("Project", nullptr, ImGuiWindowFlags_NoMove);

            // Gestion du backGround
            ImGui::ColorEdit3("BackGround", (float*)&mClearColor);
            SDL_SetRenderDrawColor(
                renderer,
                (Uint8)(mClearColor.x * 255),
                (Uint8)(mClearColor.y * 255),
                (Uint8)(mClearColor.z * 255),
                255
            );

            ImGui::Separator();

            /*
                Choisir le nom de la scene (L'option rename n'est pas pris en charge)
                Le nom du niveau est celui donner lors de la premiere sauvegarde
            */ 
            ImGui::InputText("Scene", &Level::sFileName);

            ImGui::Separator();

            // Gestion de l'eclairage globale de la scene (nuit ou jour)
            ImGui::Text("Global Lightning");
            ImGui::SliderInt("Intensity", &Level::sGlobalLightIntensity, 0, 255);
            ImGui::SliderInt("Ambient", &Level::sAmbient, 0, 255);
            
            ImGui::Separator();

            ImGui::Text("Actions");
            // mode selection de tile deja pose dans la scene
            ImGui::Checkbox("Mode Selection", &Level::sModeSelection);
            
            // Activer ou desactiver le Rendu monocouche
            ImGui::Checkbox("Single Rendering", &Level::sSingleLayerRendering);

            // Liste deroulante pour la selection de la couche courante
            if(ImGui::BeginCombo("Layer", Level::sLayers[Level::sCurrentLayerID].mName.c_str()))
            {
                for (int i = 0; i < Level::sLayerCount; i++)
                {
                    bool selected = (Level::sCurrentLayerID == i);

                    if(ImGui::Selectable(Level::sLayers[i].mName.c_str(), selected)) Level::sCurrentLayerID = i;
                    if(selected) ImGui::SetItemDefaultFocus();
                }

                ImGui::EndCombo();
            }

            ImGui::Separator();
            
            ImGui::Text("Tiles Picker");
            // Afficher dans la fenetre ImGui toutes les images presentes dans les Assets en les rendant selectionnable (TileSet)
            ImGui::Columns(COLUMNS, nullptr, false);

            for(const auto& tile : Level::sTileSet)
            {
                ImTextureID texID = (ImTextureID)tile.mTexture;

                bool selected = (tile.mID == Level::sCurrentTileID);
                
                if(ImGui::ImageButton(tile.mName.c_str(), texID, ImVec2(TILE_SIZE, TILE_SIZE), ImVec2(0, 0), ImVec2(1, 1), selected ? ImVec4(1, 1, 0, 1) : ImVec4(0, 0, 0, 1)))
                {
                    Level::sCurrentTileID = tile.mID;
                }

                ImGui::NextColumn();
            }

            ImGui::Columns(1);
            ImGui::End();
        }

        /*
            Fenetre pour L'inspection et la modification d'une tile
            Modification de :
            - La postion
            - Nom
        */ 

        {
            ImGui::SetNextWindowPos(ImVec2(1670, 20));
            ImGui::SetNextWindowSize(ImVec2(250, 540));

            ImGui::Begin("Tile Inspector", nullptr, ImGuiWindowFlags_NoMove);
            
            Inspector();

            ImGui::End();
        }

        /*
            Fenetre pour L'inspection et la modification d'une light
            Modification de :
            - La couleur
            - La postion
            - status d'activation
            - Nom
        */ 
        {
            ImGui::SetNextWindowPos(ImVec2(1670, 540));
            ImGui::SetNextWindowSize(ImVec2(250, 540));

            ImGui::Begin("Lightning Settings", nullptr, ImGuiWindowFlags_NoMove);

            if(Level::sLights.size() > 0)
            {
                LightingSettings();
            }

            ImGui::End();
        }

        /*
            Liste des lights presentent dans la scene sous forme de bouttons
        */

        {
            ImGui::SetNextWindowPos(ImVec2(0, 540));
            ImGui::SetNextWindowSize(ImVec2(250, 540));

            ImGui::Begin("Local Lighning", nullptr, ImGuiWindowFlags_NoMove);

            LightHierarchie();

            ImGui::End();
        }
        
        // Rendu
        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    }

    void GUI::Inspector()
    {
        ImGui::Separator();

        // Afficher la tile selectionner dans la scene
        if(Level::sModeSelection && Level::sLayers[Level::sCurrentLayerID].mHierarchie.size() > 0)
        {
            Level::ResetSelectionStatus();

            /*
                Recuperation de la tile selectionnee a partir de sIndex
             */
            TileInScene& selectedTile = Level::sLayers[Level::sCurrentLayerID].mHierarchie[Level::sIndex];
            selectedTile.mIsSelected = true;

            int id = selectedTile.mID;

            // index ici corresspond a l'index (dans sTileSet) de la texture que l'on va afficher
            int index = Level::FindID(id);

            // Position dans la grille
            int x = selectedTile.mGridX;
            int y = selectedTile.mGridY;

            // Supprimer une tile en fonction de sa position dans la grille
            if(ImGui::Button("Suppr"))
            {
                Level::RemoveTile(x, y);
            }

            // Afficher la texture
            ImTextureID tex = (ImTextureID)Level::sTileSet[index].mTexture;
            ImGui::Image(tex, ImVec2(4*TILE_SIZE, 4*TILE_SIZE));

            // Afficher son nom dans la scene (modifiable) et l'id de l'image (texture) dont il decoule
            ImGui::InputText("Name", &selectedTile.mName);
            ImGui::Text("ID %d", id);

            // Definir les limites de position
            const int limitX = Level::sGridWidth * Level::sTileSize - Level::sTileSize;
            const int limitY = Level::sGridHeight * Level::sTileSize - Level::sTileSize;

            // SliderFloat pour la modification des positions de la tile
            ImGui::Text("Position" );
            ImGui::SliderFloat("X ", &selectedTile.mPosX, 0, limitX);
            ImGui::SliderFloat("Y ", &selectedTile.mPosY, 0, limitY);

            ImGui::Text("Components");

            if(selectedTile.mIsAnimating)
            {
                ImGui::Text("Animation");
                ImGui::Checkbox("Loop", &selectedTile.mAnim.mLoop);
                ImGui::Text("Frame Count %d", selectedTile.mAnim.mFrameCount);
                ImGui::Text("Current Frame %d", selectedTile.mAnim.mCurrentFrame);
            }
        }

        // Afficher la tile selectionnee du tableau sTileSet
        if(Level::sCurrentTileID != -1 && !Level::sModeSelection)
        {
            int index = Level::FindID(Level::sCurrentTileID);

            // Texture
            ImTextureID tex = (ImTextureID)Level::sTileSet[index].mTexture;
            ImGui::Image(tex, ImVec2(4*TILE_SIZE, 4*TILE_SIZE));

            // nom et id
            std::string texName = Level::sTileSet[index].mName;
            int id = Level::sTileSet[index].mID;

            ImGui::Text("%s", texName.c_str());
            ImGui::Text("ID %d", id);
        }
    }

    void GUI::LightHierarchie()
    {
        if(Level::sLights.size() == 0)
        {
            return;
        }

        // Liste des lumieres dans la scene sous forme de button
        for(size_t i = 0; i < Level::sLights.size(); i++)
        {
            ImGui::Separator();

            if(ImGui::Button(Level::sLights[i].mName.c_str()))
            {
                mIndex = i;
            }

            ImGui::Separator();
        } 
    }

    void GUI::LightingSettings()
    {
        // Supprimer une lumiere en fonction de son index dans le tableau Level::sLight
        if(ImGui::Button("Supprimer"))
        {
            Level::RemoveLight(mIndex);
            mIndex = 0;
        }

        // Activer ou desactiver la lumiere
        ImGui::Checkbox("A", &Level::sLights[mIndex].mActivate);
        ImGui::SameLine();

        // Modification du nom de la lumiere
        ImGui::InputText("Name", &Level::sLights[mIndex].mName);

        ImGui::Text("Couleur et intensite");

        /*
            Modification de la couleur des rayons lumineux
            avec le choix de les afficher ou non (bool mIsRayonnant)
        */
        int x = Level::sLights[mIndex].mColorMod.r;
        int y = Level::sLights[mIndex].mColorMod.g;
        int z = Level::sLights[mIndex].mColorMod.b;

        ImGui::Checkbox("Rayonnant", &Level::sLights[mIndex].mIsRayonnant);
        ImGui::SliderInt("R ", &x, 0, 255);
        ImGui::SliderInt("G ", &y, 0, 255);
        ImGui::SliderInt("B ", &z, 0, 255);

        Level::sLights[mIndex].mColorMod.r = x;
        Level::sLights[mIndex].mColorMod.g = y;
        Level::sLights[mIndex].mColorMod.b = z;

        // Definir les limites de position de la lumiere dans la scene
        const int limitX = Level::sGridWidth * Level::sTileSize - Level::sLightSize;
        const int limitY = Level::sGridHeight * Level::sTileSize - Level::sLightSize;
        
        // SliderInt pour la modification des positions
        ImGui::Text("Position" );
        ImGui::SliderInt("X ", &Level::sLights[mIndex].mFixedPosX, 0, limitX);
        ImGui::SliderInt("Y ", &Level::sLights[mIndex].mFixedPosY, 0, limitY);  
    }
}
