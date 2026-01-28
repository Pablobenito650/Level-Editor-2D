#include "GUI.h"

namespace minieditor
{
    void GUI::GUISet(SDL_Window* window, SDL_Renderer* renderer)
    {
        // Initialisation de Dear ImGUI
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
        ImGui_ImplSDLRenderer3_Init(renderer);
    }

    void GUI::TestGUI(SDL_Renderer* renderer)
    {
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        {
            // Menu Principale
            if(ImGui::BeginMainMenuBar())
            {
                if(ImGui::BeginMenu("File"))
                {
                    if(ImGui::MenuItem("Save", "Ctrl+S"))
                    {
                        Level::SaveLevel();
                    }

                    ImGui::Separator();
                    if(ImGui::MenuItem("Load", "Ctrl+O"))
                    {
                        Level::LoadLevel();
                    }

                    ImGui::EndMenu();
                }

                if(ImGui::BeginMenu("GameObject"))
                {
                    if(ImGui::MenuItem("Light"))
                    {
                        Level::AddLight(renderer);
                    }

                    ImGui::EndMenu();
                }

                ImGui::EndMainMenuBar();
            }
            // Creation de la fenetre Dear ImGUI
            ImGui::Begin("Project");

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

            // Choisir le nom de la scene
            ImGui::InputText("Scene", &Level::sFileName);

            ImGui::Separator();

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
            // Afficher dans la fenetre ImGui toutes les images presentes dans les Assets en les rendant selectionnable (tileSet)
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

        {
            ImGui::Begin("Inspector");
            
            DrawSelectedTile();

            ImGui::End();
        }

        {
            ImGui::Begin("Local Lighning");

            DrawLightSettings();

            ImGui::End();
        }
        
        // Rendu
        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    }

    void GUI::DrawSelectedTile()
    {
        // Afficher la tile selectionner dans la scene
        if(Level::sModeSelection && Level::sLayers[Level::sCurrentLayerID].mHierarchie.size() > 0)
        {
            Level::ResetSelectionStatus();

            TileInScene& selectedTile = Level::sLayers[Level::sCurrentLayerID].mHierarchie[Level::sIndex];
            selectedTile.mIsSelected = true;

            int id = selectedTile.mID;
            int index = Level::FindID(id);

            // Afficher sa position dans le plan
            int x = selectedTile.mGridX;
            int y = selectedTile.mGridY;

            if(ImGui::Button("Suppr"))
            {
                Level::RemoveTile(x, y);
            }

            ImGui::Separator();

            // Afficher l'image definissant la tuile
            ImTextureID tex = (ImTextureID)Level::sTileSet[index].mTexture;
            ImGui::Image(tex, ImVec2(4*TILE_SIZE, 4*TILE_SIZE));

            ImGui::Separator();

            // Afficher son nom dans la scene (modifiable) et l'id de l'image (texture) dont il decoule
            ImGui::InputText("Name", &selectedTile.mName);
            ImGui::Text("ID %d", id);

            ImGui::Separator();

            ImGui::Text("Transform");
            ImGui::Text("Position X %d  Y %d", x * Level::sTileSize, y * Level::sTileSize);
        }

        // Afficher la tile (a placee) selectionnee en grand avec plus de detail
        if(Level::sCurrentTileID != -1 && !Level::sModeSelection)
        {
            int index = Level::FindID(Level::sCurrentTileID);

            // Texture
            ImTextureID tex = (ImTextureID)Level::sTileSet[index].mTexture;
            ImGui::Image(tex, ImVec2(4*TILE_SIZE, 4*TILE_SIZE));

            // nom et id
            std::string texName = Level::sTileSet[index].mName;
            int id = Level::sTileSet[index].mID;

            ImGui::Separator();

            ImGui::Text("%s", texName.c_str());
            ImGui::Text("ID %d", id);
        }
    }

    void GUI::DrawLightSettings()
    {
        if(Level::sLights.size() > 0)
        {
            ImGui::Checkbox("A", &Level::sLights[0].mActivate);
            ImGui::SameLine();
            ImGui::InputText("Name", &Level::sLights[0].mName);

            ImGui::Separator();

            ImGui::Text("Couleur et intensite");

            int x = Level::sLights[0].mColorMod.r;
            int y = Level::sLights[0].mColorMod.g;
            int z = Level::sLights[0].mColorMod.b;

            ImGui::Checkbox("Rayonnant", &Level::sLights[0].mIsRayonnant);
            ImGui::SliderInt("R ", &x, 0, 255);
            ImGui::SliderInt("G ", &y, 0, 255);
            ImGui::SliderInt("B ", &z, 0, 255);

            Level::sLights[0].mColorMod.r = x;
            Level::sLights[0].mColorMod.g = y;
            Level::sLights[0].mColorMod.b = z;

            ImGui::Separator();

            const int limitX = Level::sGridWidth * Level::sTileSize - Level::sLightSize;
            const int limitY = Level::sGridHeight * Level::sTileSize - Level::sLightSize;
        
            ImGui::Text("Position" );
            ImGui::SliderInt("X ", &Level::sLights[0].mFixedPosX, 0, limitX);
            ImGui::SliderInt("Y ", &Level::sLights[0].mFixedPosY, 0, limitY);
        }
    }
}
