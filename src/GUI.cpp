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

            ImGui::Text("File");
            // Choisir le nom de la scene
            ImGui::InputText("Scene", &Level::mfileName);

            // Sauvegarder la scene courante
            if(ImGui::Button("Save"))
            {
                Level::SaveLevel();
            }

            ImGui::SameLine();
            // Charger une scene
            if(ImGui::Button("Load"))
            {
                Level::LoadLevel();
            }
            
            ImGui::Text("Actions");
            // Activer ou desactiver le mode suppression de tiles
            if(ImGui::Checkbox("Remove Tile", &Level::mRemove))
            {

            }
            
            // Activer ou desactiver le Rendu monocouche
            if(ImGui::Checkbox("Single Rendering", &Level::mSingleLayerRendering))
            {

            }

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
            
            ImGui::Text("Tiles Picker");
            // Afficher dans la fenetre ImGui toutes les images presentes dans les Assets en les rendant selectionnable
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

            // Afficher la tile selectionnee en grand avec plus de detail
            if(Level::mcurrentTileID != -1)
            {
                ImTextureID _tex = (ImTextureID)Level::mtileSet[Level::mcurrentTileID].texture;
                ImGui::Image(_tex, ImVec2(4*mtileSize, 4*mtileSize));

                std::string _texName = Level::mtileSet[Level::mcurrentTileID].name;
                int _id = Level::mtileSet[Level::mcurrentTileID].tileID;

                ImGui::Text("%s", _texName.c_str());
                ImGui::Text("ID %d", _id);
            }

            ImGui::End();
        }
        // Rendu
        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), mRenderer);
    }
}
