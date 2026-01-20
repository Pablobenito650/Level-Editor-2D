#include "GUI.h"

namespace minieditor
{
    void GUI::GUISet(SDL_Window* window, SDL_Renderer* mRenderer)
    {
        // Initialisation de Dear ImGUI
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGui_ImplSDL3_InitForSDLRenderer(window, mRenderer);
        ImGui_ImplSDLRenderer3_Init(mRenderer);
    }

    void GUI::TestGUI(SDL_Renderer* mRenderer)
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

                ImGui::EndMainMenuBar();
            }
            // Creation de la fenetre Dear ImGUI
            ImGui::Begin("Project");

            // Gestion du backGround
            ImGui::ColorEdit3("BackGround", (float*)&mclearColor);
            SDL_SetRenderDrawColor(
                mRenderer,
                (Uint8)(mclearColor.x * 255),
                (Uint8)(mclearColor.y * 255),
                (Uint8)(mclearColor.z * 255),
                255
            );

            ImGui::Separator();

            // Choisir le nom de la scene
            ImGui::InputText("Scene", &Level::mfileName);

            ImGui::Separator();

            ImGui::Text("Global Lightning");
            ImGui::SliderInt("Intensity", &Level::mglobalLightIntensity, 0, 255);
            ImGui::SliderInt("Ambient", &Level::mambient, 0, 255);
            
            ImGui::Separator();

            ImGui::Text("Actions");
            // mode selection de tile deja pose dans la scene
            ImGui::Checkbox("Mode Selection", &Level::mmodeSelection);
            
            // Activer ou desactiver le Rendu monocouche
            ImGui::Checkbox("Single Rendering", &Level::mSingleLayerRendering);

            // Liste deroulante pour la selection de la couche courante
            if(ImGui::BeginCombo("Layer", Level::mLayers[Level::mcurrentLayerID].name.c_str()))
            {
                for (int i = 0; i < Level::mLayerCount; i++)
                {
                    bool selected = (Level::mcurrentLayerID == i);

                    if(ImGui::Selectable(Level::mLayers[i].name.c_str(), selected)) Level::mcurrentLayerID = i;
                    if(selected) ImGui::SetItemDefaultFocus();
                }

                ImGui::EndCombo();
            }

            ImGui::Separator();
            
            ImGui::Text("Tiles Picker");
            // Afficher dans la fenetre ImGui toutes les images presentes dans les Assets en les rendant selectionnable (tileSet)
            ImGui::Columns(mcolumns, nullptr, false);

            for(const auto& tile : Level::mtileSet)
            {
                ImTextureID _texID = (ImTextureID)tile.texture;

                bool _selected = (tile.tileID == Level::mcurrentTileID);
                
                if(ImGui::ImageButton(tile.name.c_str(), _texID, ImVec2(mtileSize, mtileSize), ImVec2(0, 0), ImVec2(1, 1), _selected ? ImVec4(1, 1, 0, 1) : ImVec4(0, 0, 0, 1)))
                {
                    Level::mcurrentTileID = tile.tileID;
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

            ImGui::InputText("Name", &Level::mLights[0].mName);

            ImGui::Separator();

            ImGui::Text("Couleur et intensite");

            int x = Level::mLights[0].colorMod.r;
            int y = Level::mLights[0].colorMod.g;
            int z = Level::mLights[0].colorMod.b;

            ImGui::SliderInt("R ", &x, 0, 255);
            ImGui::SliderInt("G ", &y, 0, 255);
            ImGui::SliderInt("B ", &z, 0, 255);

            Level::mLights[0].colorMod.r = x;
            Level::mLights[0].colorMod.g = y;
            Level::mLights[0].colorMod.b = z;

            ImGui::Separator();
            
            ImGui::Text("Position");
            ImGui::SliderFloat("X ", &Level::mLights[0].props.x, 0, 1024);
            ImGui::SliderFloat("Y ", &Level::mLights[0].props.y, 0, 992);

            ImGui::End();
        }
        
        // Rendu
        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), mRenderer);
    }

    void GUI::DrawSelectedTile()
    {
        // Afficher la tile selectionner dans la scene
        if(Level::mmodeSelection && Level::mLayers[Level::mcurrentLayerID].hierarchie.size() > 0)
        {
            Level::ResetSelectionStatus();

            TileInScene& selectedTile = Level::mLayers[Level::mcurrentLayerID].hierarchie[Level::mindex];
            selectedTile.mIsSelected = true;

            int _id = selectedTile.mID;
            int _index = Level::FindID(_id);

            // Afficher sa position dans le plan
            int x = selectedTile.mgridX;
            int y = selectedTile.mgridY;

            if(ImGui::Button("Suppr"))
            {
                Level::RemoveTile(x, y);
            }

            ImGui::Separator();

            // Afficher l'image definissant la tuile
            ImTextureID _tex = (ImTextureID)Level::mtileSet[_index].texture;
            ImGui::Image(_tex, ImVec2(4*mtileSize, 4*mtileSize));

            ImGui::Separator();

            // Afficher son nom dans la scene (modifiable) et l'id de l'image (texture) dont il decoule
            ImGui::InputText("Name", &selectedTile.mName);
            ImGui::Text("ID %d", _id);

            ImGui::Separator();

            ImGui::Text("Transform");
            ImGui::Text("Position X %d  Y %d", x * Level::mtileSize, y * Level::mtileSize);
            // Afficher et gere le scale de la tuile dans la scene
            ImGui::SliderFloat("Scale", &selectedTile.size, 0.5f, 2.0f);
        }

        // Afficher la tile (a placee) selectionnee en grand avec plus de detail
        if(Level::mcurrentTileID != -1 && !Level::mmodeSelection)
        {
            int _index = Level::FindID(Level::mcurrentTileID);

            // Texture
            ImTextureID _tex = (ImTextureID)Level::mtileSet[_index].texture;
            ImGui::Image(_tex, ImVec2(4*mtileSize, 4*mtileSize));

            // nom et id
            std::string _texName = Level::mtileSet[_index].name;
            int _id = Level::mtileSet[_index].tileID;

            ImGui::Separator();

            ImGui::Text("%s", _texName.c_str());
            ImGui::Text("ID %d", _id);
        }
    }
}
