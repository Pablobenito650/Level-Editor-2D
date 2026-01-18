#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>
#include <vector>
#include <filesystem>
#include <fstream>

namespace minieditor
{
    /**
     * @brief structure pour les tiles aidant a l'ecriture du fichier de mapping Tileset.txt
     */
    struct TileForTS
    {
        int mID = 0;
        std::string mName;
    };

    /**
     * @brief structure pour les tiles placees dans la scene
     */
    struct TileInScene : TileForTS
    {
        int mgridX;
        int mgridY;
        float size;
    };

    /**
     * @brief structure pour les tiles dans le tile piker 
     */
    struct Tile
    {
        int tileID = 0;
        std::string name;
        SDL_Texture* texture;          
    };

    /**
     * @brief les differentes couches du niveau
     */
    struct Layer
    {
        std::string name;
        std::vector<std::vector<int>> Grid;
        std::vector<TileInScene> hierarchie;
    };

    /**
     * @brief Gestion des niveaux (Rendu et Actions)
     */
    class Level
    {
        public:
            static std::vector<Tile> mtileSet;
            static int mcurrentTileID;
            static int mcurrentLayerID;
            
            static std::vector<Layer> mLayers;
            std::vector<int> musedIDs;
            static std::string mfileName;
            static const std::string msavePath;
            static const std::string mtileSetPath;
            static const std::string mextension; 

            static bool mRemove;
            static bool mmodeSelection;
            static bool mIsLoading;
            static bool mSingleLayerRendering;
            

            static int mGridHeight;
            static int mGridWidth;
            static int mtileSize;
            static int mLayerCount;
            
            SDL_Event event;
            SDL_FRect mcamera = {0.0f, 0.0f, 1024.0f, 992.0f};

            std::string mtilesPath = "build/Assets/Tiles/";

            int mMouseX;
            int mMouseY; 
            static int mindex;           

            bool mPlace = false;

        public:
            /**
             * @brief Constructeur, ici pour les initialisations
             */
            Level();

            /**
             * @brief rendre le niveau a l'ecran
             * @param mRenderer rendu SDL
             */
            void Render(SDL_Renderer* mRenderer);

            /**
             * @brief mettre a jour l'editeur
             */
            void Update();

            /**
             * @brief placement une tile (enregistrement dans la grille)
             */ 
            void TilePlacement();

            /** 
             * @brief recuperatioin des coordonnees de la souris au clic 
             * @param event evenements SDL
             */             
            void MouseGetter(SDL_Event event);

            /**
             * @brief initialisation des couches
             */
            void InitLayers();

            /**
             * @brief initialisation de la liste de tiles
             * @param mRenderer Rendu SDL
             */
            void InitTiles(SDL_Renderer* mRenderer);

            /**
             * @brief sauvegarder un niveau dans un format texte (.txt)
             */
            static void SaveLevel();

            /**
             * @brief charger un niveau depuis le fichier txt
             */
            static void LoadLevel();

            /**
             * @brief ramener la camera a (0, 0)
             */
            void ResetCamera();

            /**
             * @brief verifier si un identifiant est deja utilise
             */
            bool IDIsUsed(int tileID);

            /**
             * @brief ecrire le fichier Tileset.txt (fichier qui fait le lien entre ID et iamge)
             */
            void WriteTileSet();

            /**
             * @brief recupere le numero (id) au debut du nom d'un fichier
             */
            int ExtractLeadingNumber(std::string name);

            /**
             * @brief bonne pratique : detruire les texture
             */
            void DestroyTextures();

            /**
             * @brief rechercher l'index correspondant a un id dans le tableau mtileSet
             */
            static int FindID(int tileID);

            /**
             * @brief rechercher l'index correspondant a la tuile qui a pour coordonne (x,y) dans la grille hierarchique (hierarchie) de la couche d'id layerID 
             */
            static int FindIDInScene(int layerID, int x, int y);
    };
} // namespace minieditor



